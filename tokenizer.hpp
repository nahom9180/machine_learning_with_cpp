#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <cstddef>

class Tokenizer {
public:
    // Load from binary format (tokens.bin)
    bool load_vocab_bin(const std::string& bin_path);

    // Keep legacy text loader in case you still need vocab.txt
    bool load_vocab(const std::string& vocab_filepath);

    int get_id(const std::string& word) const;
    std::string get_token(int id) const;
    size_t vocab_size() const;

private:
    std::unordered_map<std::string, int> word_to_id_map;
    std::vector<std::string> id_to_word_vec;
};