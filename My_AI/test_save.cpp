#include <iostream>
#include <cassert>
#include <cmath>
#include <filesystem>
#include "core/config.hpp"
#include "core/tokenizer.hpp"
#include "core/embedding_store.hpp"
#include "core/helen.hpp"

#define COLOR_GREEN "\033[1;32m"
#define COLOR_RESET "\033[0m"

void log_pass(const std::string& name) {
    std::cout << "  [" << COLOR_GREEN << "PASS" << COLOR_RESET << "] " << name << "\n";
}

int main() {
    std::cout << "\n--- SUITE 3: DISK MUTATION & PERSISTENCE TESTS ---\n";

    std::string test_embed_path  = "output/test_mutation_embeddings.bin";
    std::string test_weight_path = "output/test_mutation_weights.bin";

    // =========================================================================
    // 1. EMBEDDING STORE: TARGETED VECTOR MUTATION & RECOVERY
    // =========================================================================
    {
        // A. Load existing real embeddings
        EmbeddingStore store_writer(0, CFG_DIMENSIONS);
        assert(store_writer.load_from_file("output/embeddings.bin"));
        size_t vocab = store_writer.get_vocab_size();
        assert(vocab >= 3);

        // Record neighbor tokens to verify they don't get corrupted
        float neighbor_val_before = store_writer.get_vector(0)[0];
        float neighbor_val_after  = store_writer.get_vector(2)[0];

        // B. Inject known "watermark" into Token #1
        float* target_vec = store_writer.get_vector(1);
        for (size_t d = 0; d < CFG_DIMENSIONS; ++d) {
            target_vec[d] = 777.0f + static_cast<float>(d) * 1.5f;
        }

        // C. Save to disk
        assert(store_writer.save_to_file(test_embed_path));

        // D. Verify file size matches expected layout exactly: Header (16B) + Vocab * Dim * 4B
        size_t expected_bytes = 16 + (vocab * CFG_DIMENSIONS * sizeof(float));
        size_t actual_bytes = std::filesystem::file_size(test_embed_path);
        assert(actual_bytes == expected_bytes);
        log_pass("Embeddings byte-size invariance (exact file size, no byte-drift)");

        // E. Cold read into a completely brand new instance
        EmbeddingStore store_reader(0, CFG_DIMENSIONS);
        assert(store_reader.load_from_file(test_embed_path));

        // Verify the injected watermark on Token #1 was saved accurately
        const float* read_vec = store_reader.get_vector(1);
        for (size_t d = 0; d < CFG_DIMENSIONS; ++d) {
            float expected_val = 777.0f + static_cast<float>(d) * 1.5f;
            assert(std::abs(read_vec[d] - expected_val) < 1e-5f);
        }
        log_pass("Embeddings targeted mutation & exact cold-reload");

        // F. Zero-Corruption check: confirm Token #0 and Token #2 were NOT touched
        assert(std::abs(store_reader.get_vector(0)[0] - neighbor_val_before) < 1e-5f);
        assert(std::abs(store_reader.get_vector(2)[0] - neighbor_val_after) < 1e-5f);
        log_pass("Embeddings zero-corruption check (adjacent tokens remain untouched)");

        // Clean up test file
        std::filesystem::remove(test_embed_path);
    }

    // =========================================================================
    // 2. KNOWLEDGE CLUSTER: 2D & 3D WEIGHTS MUTATION & PERSISTENCE
    // =========================================================================
    {
        KnowledgeClusterCpp cluster_writer;
        assert(cluster_writer.load_from_file("output/weights.bin"));

        // A. Mutate a specific 2D matrix entry: Row 2, Col 4
        cluster_writer.set_weight(2, 4, 888.123f);

        // B. Mutate a specific 3D layer entry: Layer 1, Token 10
        Vector custom_vec = {};
        custom_vec[0] = -333.444f;
        custom_vec[CFG_DIMENSIONS - 1] = 999.555f;
        cluster_writer.set_token(1, 10, custom_vec);

        // C. Save to disk
        assert(cluster_writer.save_to_file(test_weight_path));

        // D. Verify file size matches Matrix2d + Matrix3d exactly
        size_t m2_bytes = CFG_DIMENSIONS * CFG_DIMENSIONS * sizeof(float);
        size_t m3_bytes = CFG_LAYERS * CFG_TOKENS * CFG_DIMENSIONS * sizeof(float);
        assert(std::filesystem::file_size(test_weight_path) == (m2_bytes + m3_bytes));
        log_pass("Weights byte-size invariance (exact memory footprint preserved)");

        // E. Cold read into a fresh cluster instance
        KnowledgeClusterCpp cluster_reader;
        assert(cluster_reader.load_from_file(test_weight_path));

        // Check 2D mutated weight
        assert(std::abs(cluster_reader.get_weight(2, 4) - 888.123f) < 1e-4f);
        // Check untouched 2D neighbor (should remain 0.0)
        assert(cluster_reader.get_weight(2, 3) == 0.0f);

        // Check 3D mutated layer vector
        Vector loaded_vec = cluster_reader.get_token(1, 10);
        assert(std::abs(loaded_vec[0] - (-333.444f)) < 1e-4f);
        assert(std::abs(loaded_vec[CFG_DIMENSIONS - 1] - 999.555f) < 1e-4f);
        log_pass("Weights 2D & 3D targeted mutation and exact cold-reload");

        // Clean up test file
        std::filesystem::remove(test_weight_path);
    }

    std::cout << COLOR_GREEN << "✔ Disk mutation and persistence tests passed successfully!\n" << COLOR_RESET;
    return 0;
}
