#include <complex>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>
#include <chrono>
#include <random>

#include "core/config.hpp"
#include "core/tokenizer.hpp"
#include "core/embedding_store.hpp"
#include "core/helen.hpp"
#include "core/math.hpp"

#define COLOR_GREEN "\033[1;32m"
#define COLOR_RESET "\033[0m"

struct Trigram {
    int prev_id;
    int curr_id;
    int target_id;
};

int main() {
    std::cout << "\n============================================================\n";
    std::cout << "      HELEN AI: CONTRASTIVE NEGATIVE-SAMPLING TRAINER       \n";
    std::cout << "============================================================\n";

    // 1. Load Tokenizer & Storage
    Tokenizer tokenizer;
    if (!tokenizer.load_vocab_bin("output/tokens.bin")) return 1;

    // Get the vocabulary size dynamically before passing it to EmbeddingStore
    size_t vocab_size = tokenizer.vocab_size();

    EmbeddingStore store(vocab_size, CFG_DIMENSIONS);
    if (!store.load_from_file("output/embeddings.bin")) return 1;

    KnowledgeClusterCpp cluster;
    if (!cluster.load_from_file("output/weights.bin")) return 1;

    // 2. Parse Clean Dataset
    std::ifstream dataset("data/dataset.txt");
    std::vector<Trigram> dataset_trigrams;
    std::string line;

    while (std::getline(dataset, line)) {
        std::stringstream ss(line);
        std::string word;
        std::vector<int> ids;
        while (ss >> word) {
            ids.push_back(tokenizer.get_id(word));
        }
        for (size_t i = 1; i + 1 < ids.size(); ++i) {
            dataset_trigrams.push_back({ ids[i - 1], ids[i], ids[i + 1] });
        }
    }
    dataset.close();

    std::cout << "[Init] Loaded " << vocab_size << " clean tokens.\n";
    std::cout << "[Dataset] Training on " << dataset_trigrams.size() << " transitions.\n\n";

    // 3. Initialize Target Embeddings
    for (size_t t = 0; t < vocab_size; ++t) {
        const float* raw = store.get_vector(t);
        Vector v = {};
        for (size_t d = 0; d < CFG_DIMENSIONS; ++d) v[d] = raw[d];
        cluster.set_token(1, t, HelenMath::normalize(v));
    }

    // 4. Hyperparameters
    const int TOTAL_EPOCHS   = 80;
    const float INITIAL_LR   = 0.05f;
    const float FINAL_LR     = 0.005f;
    const int NEGATIVE_SAMPLES = 4; // 4 random words pushed away for every 1 correct word

    std::mt19937 rng(1337);
    std::uniform_int_distribution<int> rand_vocab(0, vocab_size - 1);

    auto start_time = std::chrono::high_resolution_clock::now();

    // 5. Training Loop
    for (int epoch = 1; epoch <= TOTAL_EPOCHS; ++epoch) {
        float progress = static_cast<float>(epoch - 1) / static_cast<float>(TOTAL_EPOCHS);
        float lr = INITIAL_LR - progress * (INITIAL_LR - FINAL_LR);
        float total_loss = 0.0f;

        for (const auto& tri : dataset_trigrams) {
            float* prev_raw = store.get_vector(tri.prev_id);
            float* curr_raw = store.get_vector(tri.curr_id);

            // Context: 0.5 * previous + 1.0 * current
            Vector context = {};
            for (size_t d = 0; d < CFG_DIMENSIONS; ++d) {
                context[d] = 0.5f * prev_raw[d] + curr_raw[d];
            }
            context = HelenMath::normalize(context);

            Vector pred = HelenMath::normalize(cluster.transform(context));

            // =========================================================
            // A. POSITIVE PULL: Attract true target (target should match pred)
            // =========================================================
            Vector target_pos = cluster.get_token(1, tri.target_id);
            float pos_dot = HelenMath::dot(target_pos, pred);
            float pos_err = 1.0f - pos_dot; // Loss: how far from +1.0
            total_loss += pos_err * pos_err;

            // Gradient: pull target and pred together
            Vector pos_delta = HelenMath::scale(pred, lr * pos_err);
            cluster.set_token(1, tri.target_id, HelenMath::normalize(HelenMath::add(target_pos, pos_delta)));

            // Update W
            Vector w_err = HelenMath::sub(target_pos, pred);
            cluster.do_outer_product_update(w_err, context, lr * 0.5f);

            // =========================================================
            // B. NEGATIVE PUSH (Inhibition): Repel 4 random unrelated words!
            // =========================================================
            for (int k = 0; k < NEGATIVE_SAMPLES; ++k) {
                int neg_id = rand_vocab(rng);
                if (neg_id == tri.target_id) continue;

                Vector target_neg = cluster.get_token(1, neg_id);
                float neg_dot = HelenMath::dot(target_neg, pred);

                // If negative word is anywhere near the prediction, PUSH IT AWAY!
                if (neg_dot > -0.2f) {
                    float push_force = lr * (neg_dot - (-0.2f));
                    Vector neg_delta = HelenMath::scale(pred, -push_force); // Negative sign = REPEL
                    cluster.set_token(1, neg_id, HelenMath::normalize(HelenMath::add(target_neg, neg_delta)));
                }
            }

            // =========================================================
            // C. Backprop into Input Embeddings
            // =========================================================
            Vector backprop_dx = cluster.transform_transpose(w_err);
            for (size_t d = 0; d < CFG_DIMENSIONS; ++d) {
                curr_raw[d] += lr * 0.5f * backprop_dx[d];
                prev_raw[d] += lr * 0.25f * backprop_dx[d];
            }
        }

        if (epoch == 1 || epoch % 10 == 0 || epoch == TOTAL_EPOCHS) {
            std::cout << "Epoch [" << std::setw(2) << epoch << "/" << TOTAL_EPOCHS << "] "
            << "Loss: " << std::fixed << std::setprecision(4)
            << (total_loss / dataset_trigrams.size()) << "\n";
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();

    std::cout << COLOR_GREEN << "\n✔ Contrastive training finished in "
    << (ms / 1000.0) << " seconds!\n" << COLOR_RESET;

    store.save_to_file("output/embeddings.bin");
    cluster.save_to_file("output/weights.bin");
    return 0;
}
