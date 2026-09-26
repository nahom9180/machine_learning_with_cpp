#include <iostream>
#include <iomanip> // manipulates how data appears when printed
#include "tokenizer.hpp"
#include "embedding_store.hpp"

int main() {
    // 1. Initialize Tokenizer
    Tokenizer tokenizer;
    if (!tokenizer.load_vocab("output/vocab.txt")) {
        std::cerr << "Error: Could not load vocabulary file." << std::endl;
        return 1;
    }

    size_t vocab_size = tokenizer.vocab_size();
    size_t dim_size = 64; // Set desired vector dimension size (e.g., 64 dimensions)

    std::cout << "[Main] Vocabulary size loaded: " << vocab_size << " words.\n";

    // 2. Initialize EmbeddingStore
    EmbeddingStore store(vocab_size, dim_size);
    std::string bin_path = "output/dimensions.bin";

    if (!store.init_or_load(bin_path)) {
        std::cerr << "Error: Failed to initialize/load embedding store." << std::endl;
        return 1;
    }

    // 3. Select a word to test pointer access
    std::string test_word = "cat"; // Replace with any word in your vocab
    int token_id = tokenizer.get_id(test_word);

    std::cout << "\n--- Vector Operations Test ---\n";
    std::cout << "Word: '" << test_word << "' -> Token ID: " << token_id << "\n";

    // Grab non-const vector pointer
    float* vec = store.get_vector(token_id);
    if (vec != nullptr) {
        std::cout << "Original dimensions [0-3]: " 
                  << std::fixed << std::setprecision(4)
                  << vec[0] << ", " << vec[1] << ", " 
                  << vec[2] << ", " << vec[3] << "\n";

        // Simulate a weight update during training
        vec[0] += 1.2345f;
        
        std::cout << "Updated  dimensions [0-3]: " 
                  << vec[0] << ", " << vec[1] << ", " 
                  << vec[2] << ", " << vec[3] << "\n";
    } else {
        std::cout << "Warning: Token ID out of bounds.\n";
    }

    // 4. Save modified cluster to disk
    if (store.save_to_file(bin_path)) {
        std::cout << "\n[Shutdown] Saved updated matrix to " << bin_path << "\n";
    } else {
        std::cerr << "Error: Failed to save updated matrix to disk.\n";
        return 1;
    }

    return 0;
}