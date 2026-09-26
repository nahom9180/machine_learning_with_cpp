#include <iostream>
#include <iomanip>
#include "config.hpp" // <--- Include single config
#include "tokenizer.hpp"
#include "embedding_store.hpp"

int main() {
    Tokenizer tokenizer;
  if (!tokenizer.load_vocab_bin("output/tokens.bin")) {
    std::cerr << "Error: Could not load tokens.bin\n";
    return 1;
}

    // Now controlled centrally from config.hpp
    size_t vocab_size = tokenizer.vocab_size();
    size_t dim_size = CFG_DIMENSIONS; 

    std::cout << "[Main] Vocab: " << vocab_size << " words | Dim: " << dim_size << "\n";

    EmbeddingStore store(vocab_size, dim_size);
    std::string bin_path = "output/dimensions.bin";

    if (!store.init_or_load(bin_path)) {
        std::cerr << "Error: Failed to initialize/load embedding store." << std::endl;
        return 1;
    }

    std::string test_word = "cat";
    int token_id = tokenizer.get_id(test_word);

    std::cout << "\n--- Vector Operations Test ---\n";
    std::cout << "Word: '" << test_word << "' -> Token ID: " << token_id << "\n";

    float* vec = store.get_vector(token_id);
    if (vec != nullptr) {
        std::cout << "Original dimensions [0-3]: " 
                  << std::fixed << std::setprecision(4)
                  << vec[0] << ", " << vec[1] << ", " 
                  << vec[2] << ", " << vec[3] << "\n";

        vec[0] += 1.2345f;
        
        std::cout << "Updated  dimensions [0-3]: " 
                  << vec[0] << ", " << vec[1] << ", " 
                  << vec[2] << ", " << vec[3] << "\n";
    }

    if (store.save_to_file(bin_path)) {
        std::cout << "\n[Shutdown] Safely written to " << bin_path << "\n";
    }

    return 0;
}