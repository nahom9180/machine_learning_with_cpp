#include "embedding_store.hpp"
#include <fstream>
#include <iostream>
#include <random>

EmbeddingStore::EmbeddingStore(size_t vocab_size, size_t dim)
    : vocab_size_(vocab_size), dim_(dim), matrix_(vocab_size * dim, 0.0f) {}

// Non-const accessor: Mutable pointer for training updates
float* EmbeddingStore::get_vector(size_t token_id) {
    if (token_id >= vocab_size_) return nullptr;
    return &matrix_[token_id * dim_];
}

// Const accessor: Read-only pointer for inference
const float* EmbeddingStore::get_vector(size_t token_id) const {
    if (token_id >= vocab_size_) return nullptr;
    return &matrix_[token_id * dim_];
}

// Removed 'const' qualifier: Modifies matrix_ in memory
bool EmbeddingStore::load_from_file(const std::string& bin_filepath) {
    std::ifstream file(bin_filepath, std::ios::binary);
    if (!file.is_open()) return false;

    uint32_t file_vocab = 0;
    uint32_t file_dim = 0;

    // Read header
    file.read(reinterpret_cast<char*>(&file_vocab), sizeof(file_vocab));
    file.read(reinterpret_cast<char*>(&file_dim), sizeof(file_dim));

    if (file_vocab != vocab_size_ || file_dim != dim_) {
        std::cerr << "Dimension mismatch in bin file!\n";
        return false;
    }

    // Read the entire cluster block directly into RAM in ONE bulk operation
    size_t total_bytes = vocab_size_ * dim_ * sizeof(float);
    file.read(reinterpret_cast<char*>(matrix_.data()), total_bytes);

    file.close();
    return true;
}

// Kept 'const' qualifier: Pure read-only operation
bool EmbeddingStore::save_to_file(const std::string& bin_filepath) const {
    std::ofstream file(bin_filepath, std::ios::binary);
    if (!file.is_open()) return false;

    uint32_t file_vocab = static_cast<uint32_t>(vocab_size_);
    uint32_t file_dim = static_cast<uint32_t>(dim_);

    // Write header
    file.write(reinterpret_cast<const char*>(&file_vocab), sizeof(file_vocab));
    file.write(reinterpret_cast<const char*>(&file_dim), sizeof(file_dim));

    // Write the entire matrix block to disk in ONE bulk operation
    size_t total_bytes = vocab_size_ * dim_ * sizeof(float);
    file.write(reinterpret_cast<const char*>(matrix_.data()), total_bytes);

    file.close();
    return true;
}

// Removed 'const' qualifier: Modifies matrix_ in memory
bool EmbeddingStore::init_or_load(const std::string& bin_filepath) {
    // Try loading existing binary weights first
    if (load_from_file(bin_filepath)) {
        std::cout << "[Storage] Loaded existing cluster from " << bin_filepath << "\n";
        return true;
    }

    // File doesn't exist -> initialize with small random weights for training
    std::cout << "[Storage] Creating new embedding cluster file...\n";
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-0.1f, 0.1f);

    for (size_t i = 0; i < matrix_.size(); ++i) {
        matrix_[i] = dist(rng);
    }

    // Save initial state to file
    return save_to_file(bin_filepath);
}