/*
This is used to introduce the AI to Words for the very first time
it assigns the first 2000 (or whatever tokens per layer) most common
words delimited by space. I kept this script separate from the inference maker
to achieve separation of concerns. 
*/ 

#include <iostream>
#include <fstream>   // file stream
#include <string>
#include <unordered_map> // for key value counting
#include <vector>    // for dynamic list
#include <algorithm> // for sorting

int main() {

    std::ifstream file("dataset.txt");

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file" << std::endl;
        return 1;
    }

    std::unordered_map<std::string, int> word_counts;
    std::vector<std::pair<std::string, int>> freq_list;

    std::string word;

    while (file >> word) {
        word_counts[word]++;
    }
 
    file.close();

    // Copy each (word, count) pair from the map into the vector
    for (const auto& pair : word_counts) {
        freq_list.push_back(pair);
    }

    // Sort descending by frequency count
    std::sort(freq_list.begin(), freq_list.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    // Print the top most frequent words for debugging
    std::cout << "--- Top Words Preview ---\n";
    for (size_t i = 0; i < 20 && i < freq_list.size(); i++) {
        std::cout << "#" << i << " " << freq_list[i].first 
                  << " (" << freq_list[i].second << " times)\n";
    }
    std::cout << "-------------------------\n";

    std::ofstream out_file("vocab.txt");
    if (!out_file.is_open()) {
        std::cerr << "Error: Could not create vocab.txt" << std::endl;
        return 1;
    }

    const size_t TOKENS = 2000;
    size_t lines = std::min(freq_list.size(), TOKENS);

    for (size_t i = 0; i < lines; i++) {
        out_file << freq_list[i].first << "\n";
    }

    out_file.close();

    // Finalization report
    std::cout << "\n[Done] Wrote " << lines << " tokens to vocab.txt"
              << " (from " << freq_list.size() << " total unique words found).\n";

    return 0;
}