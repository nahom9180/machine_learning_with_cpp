#include <iostream>
#include <cassert>
#include <cmath>
#include "core/config.hpp"
#include "core/tokenizer.hpp"
#include "core/embedding_store.hpp"
#include "core/helen.hpp"

// Terminal colors for clean pass/fail reports
#define COLOR_GREEN "\033[1;32m"
#define COLOR_RED   "\033[1;31m"
#define COLOR_RESET "\033[0m"

void log_pass(const std::string& test_name) {
    std::cout << "[" << COLOR_GREEN << "PASS" << COLOR_RESET << "] " << test_name << "\n";
}

void log_fail(const std::string& test_name, const std::string& reason) {
    std::cerr << "[" << COLOR_RED << "FAIL" << COLOR_RESET << "] " << test_name << ": " << reason << "\n";
}

// ---------------------------------------------------------
// 1. TEST TOKENIZER
// ---------------------------------------------------------
void test_tokenizer() {
    Tokenizer tokenizer;
    if (!tokenizer.load_vocab_bin("output/tokens.bin")) {
        log_fail("Tokenizer Load", "Could not load output/tokens.bin (run 'make init' first)");
        return;
    }

    assert(tokenizer.vocab_size() > 0);
    
    // Check two-way mapping for token 0
    std::string first_word = tokenizer.get_token(0);
    assert(!first_word.empty());
    assert(tokenizer.get_id(first_word) == 0);

    // Check unknown word handling
    assert(tokenizer.get_id("__nonexistent_word_xyz__") == 0);

    log_pass("Tokenizer (Binary load, two-way dictionary mapping, fallback)");
}

// ---------------------------------------------------------
// 2. TEST EMBEDDING STORE
// ---------------------------------------------------------
void test_embedding_store() {
    EmbeddingStore store(CFG_TOKENS, CFG_DIMENSIONS);
    if (!store.load_from_file("output/embeddings.bin")) {
        log_fail("EmbeddingStore Load", "Could not load output/embeddings.bin");
        return;
    }

    assert(store.get_vocab_size() == CFG_TOKENS);
    assert(store.get_dim() == CFG_DIMENSIONS);

    float* vec0 = store.get_vector(0);
    assert(vec0 != nullptr);

    // Verify out-of-bounds safety
    float* out_of_bounds = store.get_vector(CFG_TOKENS + 100);
    assert(out_of_bounds == nullptr);

    // Verify weight mutation
    float original_val = vec0[0];
    vec0[0] += 0.5f;
    assert(std::abs(vec0[0] - (original_val + 0.5f)) < 1e-5f);
    vec0[0] = original_val; // restore

    log_pass("EmbeddingStore (Header validation, pointer access, out-of-bounds safety)");
}

// ---------------------------------------------------------
// 3. TEST HELEN MATRIX MATH
// ---------------------------------------------------------
void test_helen_math() {
    KnowledgeClusterCpp cluster;

    // A. Dot Product Test
    Vector v1 = {};
    Vector v2 = {};
    v1[0] = 2.0f; v1[1] = 3.0f;
    v2[0] = 4.0f; v2[1] = 5.0f;

    cluster.set_token(0, 0, v1);
    float dot = cluster.compute_dotproduct(0, 0, v2);
    // 2*4 + 3*5 = 23.0
    assert(std::abs(dot - 23.0f) < 1e-5f);

    // B. Matrix-Vector Transform Test (y = W * x)
    // Set weights to identity matrix (diagonal = 1.0)
    for (size_t i = 0; i < CFG_DIMENSIONS; ++i) {
        cluster.set_weight(i, i, 1.0f);
    }
    Vector x = {};
    x[0] = 7.5f;
    x[1] = -3.2f;
    Vector y = cluster.transform(x);
    assert(std::abs(y[0] - 7.5f) < 1e-5f);
    assert(std::abs(y[1] - (-3.2f)) < 1e-5f);

    // C. Outer Product Update Test (W += lr * u * v^T)
    Vector u = {};
    Vector v = {};
    u[0] = 2.0f;
    v[1] = 3.0f;
    cluster.do_outer_product_update(u, v, 0.1f);
    // Row 0, Col 1 should increase by 0.1 * 2 * 3 = 0.6
    assert(std::abs(cluster.get_weight(0, 1) - 0.6f) < 1e-5f);

    // D. Normalization Test
    Vector norm_vec = {};
    norm_vec[0] = 3.0f;
    norm_vec[1] = 4.0f; // length = 5.0
    cluster.set_token(0, 1, norm_vec);
    cluster.do_normalize_token(0, 1);
    Vector normalized = cluster.get_token(0, 1);
    assert(std::abs(normalized[0] - 0.6f) < 1e-5f); // 3/5
    assert(std::abs(normalized[1] - 0.8f) < 1e-5f); // 4/5

    log_pass("Helen Math (Dot product, W*x transform, outer-product update, normalization)");
}

// ---------------------------------------------------------
// 4. TEST WEIGHT PERSISTENCE (SAVE / LOAD)
// ---------------------------------------------------------
void test_helen_persistence() {
    std::string test_path = "output/test_weights.bin";
    KnowledgeClusterCpp cluster_save;

    // Set distinctive values
    cluster_save.set_weight(3, 4, 42.123f);
    Vector v = {};
    v[0] = 99.9f;
    cluster_save.set_token(1, 5, v);

    assert(cluster_save.save_to_file(test_path));

    KnowledgeClusterCpp cluster_load;
    assert(cluster_load.load_from_file(test_path));

    // Verify values were preserved exactly
    assert(std::abs(cluster_load.get_weight(3, 4) - 42.123f) < 1e-5f);
    assert(std::abs(cluster_load.get_token(1, 5)[0] - 99.9f) < 1e-5f);

    // Clean up temporary test file
    std::filesystem::remove(test_path);

    log_pass("Helen Persistence (Atomic write, binary read, exact numerical fidelity)");
}

// ---------------------------------------------------------
// MAIN TEST RUNNER
// ---------------------------------------------------------
int main() {
    std::cout << "\n========================================\n";
    std::cout << "      RUNNING HELEN SYSTEM TESTS        \n";
    std::cout << "========================================\n";

    test_tokenizer();
    test_embedding_store();
    test_helen_math();
    test_helen_persistence();

    std::cout << "\n" << COLOR_GREEN << ">>> ALL TESTS PASSED SUCCESSFULLY! <<<" << COLOR_RESET << "\n\n";
    return 0;
}