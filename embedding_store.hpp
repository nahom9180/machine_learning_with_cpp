#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <cstddef>

// Header block placed at the very start of the binary file
#pragma pack(push, 1)
struct ModelHeader {
    char magic[4] = {'H', 'E', 'L', 'N'}; // File identifier
    uint32_t version = 1;                 // Storage version
    uint32_t vocab_size = 0;              // Token count
    uint32_t dim = 0;                     // Dimension size
};
#pragma pack(pop)

class EmbeddingStore {
public:
    EmbeddingStore(size_t vocab_size, size_t dim);

    bool init_or_load(const std::string& bin_filepath);
    bool load_from_file(const std::string& bin_filepath);
    bool save_to_file(const std::string& bin_filepath) const;

    float* get_vector(size_t token_id);
    const float* get_vector(size_t token_id) const;

    size_t get_vocab_size() const { return vocab_size_; }
    size_t get_dim() const { return dim_; }

private:
    size_t vocab_size_;
    size_t dim_;
    std::vector<float> matrix_;
};