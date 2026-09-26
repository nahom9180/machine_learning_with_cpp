#include "tokenizer.hpp"
#include <fstream>
#include <iostream>
#include <filesystem> // C++17 filesystem for path debugging

bool Tokenizer::load_vocab(const std::string& vocab_filepath) {
    std::ifstream file(vocab_filepath);
    if (!file.is_open()) {
        std::cerr << "\n[Debug Error] Failed to open file!" << std::endl;
        std::cerr << "  -> Target relative path : '" << vocab_filepath << "'" << std::endl;
        std::cerr << "  -> Current working dir  : '" << std::filesystem::current_path() << "'" << std::endl;
        std::cerr << "  -> Expected full path   : '" << std::filesystem::absolute(vocab_filepath) << "'\n" << std::endl;
        return false;
    }

    word_to_id_map.clear();
    id_to_word_vec.clear();

    std::string word;
    int id = 0;

    while (file >> word) {
        word_to_id_map[word] = id;
        id_to_word_vec.push_back(word);
        id++;
    }

    file.close();
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
    return "<UNK>"; // Return unknown string if ID is out of bounds
}

size_t Tokenizer::vocab_size() const {
    return id_to_word_vec.size();
}