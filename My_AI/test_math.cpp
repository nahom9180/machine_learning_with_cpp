#include <iostream>
#include <cassert>
#include <cmath>
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
    std::cout << "\n--- SUITE 2: RIGOROUS STRESS & MATH TESTS ---\n";

    // 1. Adjoint Transpose Identity: <W*x, y> == <x, W^T*y>
    {
        KnowledgeClusterCpp cluster;
        for (size_t r = 0; r < CFG_DIMENSIONS; ++r) {
            for (size_t c = 0; c < CFG_DIMENSIONS; ++c) {
                cluster.set_weight(r, c, std::sin(r * 1.5f + c * 0.7f));
            }
        }

        Vector x = {}, y = {};
        for (size_t d = 0; d < CFG_DIMENSIONS; ++d) {
            x[d] = std::cos(d * 2.1f);
            y[d] = std::sin(d * 1.3f);
        }
        Vector Wx = cluster.transform(x);
        Vector Wt_y = cluster.transform_transpose(y);
        float dot_l = 0.0f, dot_r = 0.0f;
        for (size_t d = 0; d < CFG_DIMENSIONS; ++d) {
            dot_l += Wx[d] * y[d];
            dot_r += x[d] * Wt_y[d];
        }
        assert(std::abs(dot_l - dot_r) < 1e-4f);
        log_pass("Adjoint Transpose Identity: <W*x, y> == <x, W^T*y>");
    }

    // 2. Nearest-Neighbor Argmax
    {
        KnowledgeClusterCpp cluster;
        Vector tA = {}, tB = {};
        tA[0] = 1.0f; tB[1] = 1.0f;
        cluster.set_token(0, 5, tA);
        cluster.set_token(0, 10, tB);

        Vector qA = {}; qA[0] = 0.9f;
        assert(cluster.find_best_matching(0, qA) == 5);
        log_pass("Inference Argmax nearest-neighbor retrieval");
    }

    // 3. Layer Isolation
    {
        KnowledgeClusterCpp cluster;
        Vector v = {}; v[0] = 88.8f;
        cluster.set_token(0, 4, v);
        Vector v_layer1 = cluster.get_token(1, 4);
        for (size_t d = 0; d < CFG_DIMENSIONS; ++d) {
            assert(v_layer1[d] == 0.0f);
        }
        log_pass("Layer isolation (zero cross-layer memory corruption)");
    }

    // 4. Numerical Stability & Zero-Division Safety
    {
        KnowledgeClusterCpp cluster;
        Vector zero_vec = {};
        cluster.set_token(0, 0, zero_vec);
        cluster.do_normalize_token(0, 0);
        Vector res = cluster.get_token(0, 0);
        for (size_t d = 0; d < CFG_DIMENSIONS; ++d) {
            assert(!std::isnan(res[d]));
            assert(!std::isinf(res[d]));
        }
        log_pass("Numerical stability & zero-norm guard");
    }

    // 5. Disk Scan for Corrupted Floats
    {
        EmbeddingStore store(0, CFG_DIMENSIONS);
        assert(store.load_from_file("output/embeddings.bin"));
        for (size_t t = 0; t < store.get_vocab_size(); ++t) {
            const float* vec = store.get_vector(t);
            for (size_t d = 0; d < CFG_DIMENSIONS; ++d) {
                assert(!std::isnan(vec[d]) && !std::isinf(vec[d]));
            }
        }
        log_pass("Disk scan: output/embeddings.bin has zero NaN/Inf values");
    }

    std::cout << COLOR_GREEN << "✔ Rigorous stress-tests passed successfully!\n" << COLOR_RESET;
    return 0;
}
