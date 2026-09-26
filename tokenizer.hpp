#pragma once

#include <string>
#include <unordered_map>
#include <vector>

class Tokenizer {
public:
    bool load_vocab(const std::string& vocab_filepath);

    // Pass by reference (&) to avoid copying strings
    int get_id(const std::string& word) const;

    std::string get_token(int id) const;

    size_t vocab_size() const;

private:
    std::unordered_map<std::string, int> word_to_id_map;
    std::vector<std::string> id_to_word_vec;
};