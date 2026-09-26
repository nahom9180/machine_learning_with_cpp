#include "tokenizer.hpp"
#include <fstream>
#include <iostream>

bool Tokenizer::load_vocab_bin(const std::string& bin_path) {
    std::ifstream file(bin_path, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "[Tokenizer Error] Could not open binary file: " << bin_path << "\n";
        return false;
    }

    uint32_t count = 0;
    file.read(reinterpret_cast<char*>(&count), sizeof(count));

    if (!file.good()) return false;

    word_to_id_map.clear();
    id_to_word_vec.clear();
    word_to_id_map.reserve(count);
    id_to_word_vec.reserve(count);

    for (uint32_t i = 0; i < count; ++i) {
        uint16_t len = 0;
        file.read(reinterpret_cast<char*>(&len), sizeof(len));

        std::string word(len, '\0');
        if (len > 0) {
            file.read(&word[0], len);
        }

        word_to_id_map[word] = static_cast<int>(i);
        id_to_word_vec.push_back(word);
    }

    return file.good();
}

bool Tokenizer::load_vocab(const std::string& vocab_filepath) {
    std::ifstream file(vocab_filepath);
    if (!file.is_open()) return false;

    word_to_id_map.clear();
    id_to_word_vec.clear();

    std::string word;
    int id = 0;
    while (file >> word) {
        word_to_id_map[word] = id;
        id_to_word_vec.push_back(word);
        id++;
    }
    return true;
}

int Tokenizer::get_id(const std::string& word) const {
    auto it = word_to_id_map.find(word);
    if (it != word_to_id_map.end()) {
        return it->second;
    }
    return 0; // Default to ID 0 (<UNK>) if word is missing
}

std::string Tokenizer::get_token(int id) const {
    if (id >= 0 && id < static_cast<int>(id_to_word_vec.size())) {
        return id_to_word_vec[id];
    }
    return "<UNK>";
}

size_t Tokenizer::vocab_size() const {
    return id_to_word_vec.size();
}