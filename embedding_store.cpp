#include "embedding_store.hpp"
#include <fstream>
#include <iostream>
#include <random>
#include <filesystem>
#include <cstring>

EmbeddingStore::EmbeddingStore(size_t vocab_size, size_t dim)
    : vocab_size_(vocab_size), dim_(dim), matrix_(vocab_size * dim, 0.0f) {}

float* EmbeddingStore::get_vector(size_t token_id) {
    if (token_id >= vocab_size_) return nullptr;
    return &matrix_[token_id * dim_];
}

const float* EmbeddingStore::get_vector(size_t token_id) const {
    if (token_id >= vocab_size_) return nullptr;
    return &matrix_[token_id * dim_];
}

bool EmbeddingStore::load_from_file(const std::string& bin_filepath) {
    std::ifstream file(bin_filepath, std::ios::binary);
    if (!file.is_open()) return false;

    // 1. Read and validate header
    ModelHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(ModelHeader));

    if (std::memcmp(header.magic, "HELN", 4) != 0) {
        std::cerr << "[Storage Error] Invalid format! Not a HELN model file.\n";
        return false;
    }

    if (header.vocab_size != vocab_size_ || header.dim != dim_) {
        std::cerr << "[Storage Error] Dimension mismatch! File has ("
                  << header.vocab_size << "x" << header.dim 
                  << ") but program configured for (" 
                  << vocab_size_ << "x" << dim_ << ")\n";
        return false;
    }

    // 2. Read all floats in one bulk operation at maximum disk speed
    size_t total_bytes = vocab_size_ * dim_ * sizeof(float);
    file.read(reinterpret_cast<char*>(matrix_.data()), total_bytes);

    return file.good();
}

bool EmbeddingStore::save_to_file(const std::string& bin_filepath) const {
    // Write to a temporary file first (ATOMIC SAVE)
    std::string temp_filepath = bin_filepath + ".tmp";
    std::ofstream file(temp_filepath, std::ios::binary | std::ios::trunc);
    if (!file.is_open()) return false;

    // 1. Write Header
    ModelHeader header;
    header.vocab_size = static_cast<uint32_t>(vocab_size_);
    header.dim = static_cast<uint32_t>(dim_);
    file.write(reinterpret_cast<const char*>(&header), sizeof(ModelHeader));

    // 2. Write Matrix Bulk
    size_t total_bytes = vocab_size_ * dim_ * sizeof(float);
    file.write(reinterpret_cast<const char*>(matrix_.data()), total_bytes);

    file.close();

    // 3. Atomically swap files (Crash-safe replacement)
    std::error_code ec;
    std::filesystem::rename(temp_filepath, bin_filepath, ec);
    if (ec) {
        std::cerr << "[Storage Error] Atomic rename failed: " << ec.message() << "\n";
        return false;
    }

    return true;
}

bool EmbeddingStore::init_or_load(const std::string& bin_filepath) {
    if (load_from_file(bin_filepath)) {
        std::cout << "[Storage] Successfully loaded cluster from " << bin_filepath << "\n";
        return true;
    }

    std::cout << "[Storage] Initializing new embedding cluster file...\n";
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-0.1f, 0.1f);

    for (size_t i = 0; i < matrix_.size(); ++i) {
        matrix_[i] = dist(rng);
    }

    return save_to_file(bin_filepath);
}