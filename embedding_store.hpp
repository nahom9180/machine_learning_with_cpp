#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <cstddef>

class EmbeddingStore {
public:
    EmbeddingStore(size_t vocab_size, size_t dim);

    // Removed 'const' because these mutate matrix_
    bool init_or_load(const std::string& bin_filepath);
    bool load_from_file(const std::string& bin_filepath);

    // Kept 'const' because saving only reads matrix_
    bool save_to_file(const std::string& bin_filepath) const;

    // Non-const version: Allows updating weights during training
    float* get_vector(size_t token_id);

    // Const version: Read-only access for inference
    const float* get_vector(size_t token_id) const;

private:
    size_t vocab_size_;
    size_t dim_;
    std::vector<float> matrix_; // Flat 1D array storing all vectors sequentially
};