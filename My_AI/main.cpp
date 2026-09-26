#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>

#include "core/config.hpp"
#include "core/tokenizer.hpp"
#include "core/embedding_store.hpp"
#include "core/helen.hpp"
#include "core/math.hpp"

// Terminal Colors
#define COLOR_GREEN  "\033[1;32m"
#define COLOR_CYAN   "\033[1;36m"
#define COLOR_YELLOW "\033[1;33m"
#define COLOR_MAGENTA "\033[1;35m"
#define COLOR_RESET  "\033[0m"

struct Prediction {
    int token_id;
    float score;
};

// Predicts the top K most probable next words
std::vector<Prediction> get_top_k(
    const std::string& word, 
    const Tokenizer& tokenizer, 
    EmbeddingStore& store, 
    const KnowledgeClusterCpp& cluster, 
    int k = 3
) {
    int id = tokenizer.get_id(word);
    const float* raw_x = store.get_vector(id);
    if (!raw_x) return {};

    Vector x = {};
    for (size_t d = 0; d < CFG_DIMENSIONS; ++d) x[d] = raw_x[d];

    // Project forward: y_pred = W * x
    Vector pred_y = cluster.transform(x);

    // Score against all tokens in Layer 1 cluster memory
    std::vector<Prediction> candidates;
    candidates.reserve(tokenizer.vocab_size());

    for (size_t t = 0; t < tokenizer.vocab_size(); ++t) {
        float score = cluster.compute_dotproduct(1, t, pred_y);
        candidates.push_back({ static_cast<int>(t), score });
    }

    // Sort descending by similarity score
    size_t top_count = std::min(static_cast<size_t>(k), candidates.size());
    std::partial_sort(candidates.begin(), candidates.begin() + top_count, candidates.end(),
        [](const Prediction& a, const Prediction& b) {
            return a.score > b.score;
        });

    candidates.resize(top_count);
    return candidates;
}

// Generates continuous text step-by-step from a single seed word
std::string autocomplete_sentence(
    const std::string& seed, 
    const Tokenizer& tokenizer, 
    EmbeddingStore& store, 
    const KnowledgeClusterCpp& cluster, 
    int max_length = 7
) {
    std::string current_word = seed;
    std::string result = seed;

    for (int step = 0; step < max_length; ++step) {
        int id = tokenizer.get_id(current_word);
        const float* raw_x = store.get_vector(id);
        if (!raw_x) break;

        Vector x = {};
        for (size_t d = 0; d < CFG_DIMENSIONS; ++d) x[d] = raw_x[d];

        Vector pred_y = cluster.transform(x);
        int next_id = cluster.find_best_matching(1, pred_y);

        std::string next_word = tokenizer.get_token(next_id);
        if (next_word == "<UNK>" || next_word.empty()) break;

        result += " " + next_word;
        current_word = next_word;
    }

    return result;
}

int main() {
    std::cout << "\n============================================================\n";
    std::cout << "               HELEN AI: INTERACTIVE INFERENCE              \n";
    std::cout << "============================================================\n";

    // 1. Load Tokenizer
    Tokenizer tokenizer;
    if (!tokenizer.load_vocab_bin("output/tokens.bin")) {
        std::cerr << "Error: Could not load output/tokens.bin. Run 'make init' & 'make train' first!\n";
        return 1;
    }

    // 2. Load Embeddings
    EmbeddingStore store(0, CFG_DIMENSIONS);
    if (!store.load_from_file("output/embeddings.bin")) {
        std::cerr << "Error: Could not load output/embeddings.bin\n";
        return 1;
    }

    // 3. Load Model Weights
    KnowledgeClusterCpp cluster;
    if (!cluster.load_from_file("output/weights.bin")) {
        std::cerr << "Error: Could not load output/weights.bin\n";
        return 1;
    }

    std::cout << "✔ Model loaded successfully (" << tokenizer.vocab_size() 
              << " tokens | " << CFG_DIMENSIONS << " dimensions).\n";
    std::cout << "Type a starting word (or '" << COLOR_YELLOW << "exit" << COLOR_RESET << "' to quit).\n\n";

    std::string input;
    while (true) {
        std::cout << COLOR_CYAN << "helen> " << COLOR_RESET;
        if (!(std::cin >> input) || input == "exit" || input == "quit") {
            break;
        }

        // Check if word exists in vocabulary
        int id = tokenizer.get_id(input);
        if (id == 0 && input != tokenizer.get_token(0)) {
            std::cout << COLOR_YELLOW << "  [Warning] '" << input 
                      << "' is not in the vocabulary! Using fallback.\n" << COLOR_RESET;
        }

        // 1. Show Top 3 Predictions
        auto top_3 = get_top_k(input, tokenizer, store, cluster, 3);
        std::cout << "  " << COLOR_GREEN << "Top Predictions:" << COLOR_RESET << "\n";
        for (size_t i = 0; i < top_3.size(); ++i) {
            std::cout << "    " << (i + 1) << ". " 
                      << std::left << std::setw(12) << tokenizer.get_token(top_3[i].token_id)
                      << " (score: " << std::fixed << std::setprecision(3) << top_3[i].score << ")\n";
        }

        // 2. Autocomplete a full phrase
        std::string generated = autocomplete_sentence(input, tokenizer, store, cluster, 6);
        std::cout << "  " << COLOR_MAGENTA << "Autocompletion:" << COLOR_RESET 
                  << " \"" << generated << "\"\n\n";
    }

    std::cout << "\n[Shutdown] Goodbye!\n";
    return 0;
}