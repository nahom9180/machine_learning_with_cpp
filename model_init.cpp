#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <random>
#include <filesystem>
#include "config.hpp"
#include "embedding_store.hpp" // uses ModelHeader

int main() {
    std::filesystem::create_directories("output");

    // 1. READ RAW DATASET & COUNT FREQUENCIES
    std::ifstream dataset("output/dataset.txt");
    if (!dataset.is_open()) {
        std::cerr << "Error: Could not open dataset.txt\n";
        return 1;
    }

    std::unordered_map<std::string, int> counts;
    std::string word;
    while (dataset >> word) {
        counts[word]++;
    }
    dataset.close();

    std::vector<std::pair<std::string, int>> freq_list(counts.begin(), counts.end());
    std::sort(freq_list.begin(), freq_list.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    size_t actual_tokens = std::min(freq_list.size(), CFG_TOKENS);
    std::cout << "[Init] Discovered " << counts.size() << " unique words. Selecting top " 
              << actual_tokens << " tokens.\n";

    // 2. WRITE `output/tokens.bin` (Binary String Table)
    {
        std::ofstream token_file("output/tokens.bin", std::ios::binary);
        uint32_t count = static_cast<uint32_t>(actual_tokens);
        token_file.write(reinterpret_cast<const char*>(&count), sizeof(count));

        for (size_t i = 0; i < actual_tokens; ++i) {
            const std::string& str = freq_list[i].first;
            uint16_t len = static_cast<uint16_t>(str.size());
            token_file.write(reinterpret_cast<const char*>(&len), sizeof(len));
            token_file.write(str.data(), len);
        }
        std::cout << "[Init] -> Created output/tokens.bin\n";
    }

    // 3. WRITE `output/embeddings.bin` (Random Initial Float Matrix)
    {
        std::ofstream embed_file("output/embeddings.bin", std::ios::binary);
        ModelHeader header;
        header.vocab_size = static_cast<uint32_t>(actual_tokens);
        header.dim = static_cast<uint32_t>(CFG_DIMENSIONS);
        embed_file.write(reinterpret_cast<const char*>(&header), sizeof(ModelHeader));

        std::mt19937 rng(42);
        std::uniform_real_distribution<float> dist(-0.1f, 0.1f);

        std::vector<float> embeds(actual_tokens * CFG_DIMENSIONS);
        for (float& val : embeds) {
            val = dist(rng);
        }

        embed_file.write(reinterpret_cast<const char*>(embeds.data()), embeds.size() * sizeof(float));
        std::cout << "[Init] -> Created output/embeddings.bin\n";
    }

    // 4. WRITE `output/weights.bin` (Knowledge Cluster: Matrix2d + Matrix3d)
    {
        std::ofstream weight_file("output/weights.bin", std::ios::binary);
        
        // Matrix2d: [DIMENSIONS][DIMENSIONS]
        size_t m2_size = CFG_DIMENSIONS * CFG_DIMENSIONS;
        // Matrix3d: [LAYERS][TOKENS][DIMENSIONS]
        size_t m3_size = CFG_LAYERS * actual_tokens * CFG_DIMENSIONS;

        std::vector<float> zeros(m2_size + m3_size, 0.0f);
        weight_file.write(reinterpret_cast<const char*>(zeros.data()), zeros.size() * sizeof(float));
        std::cout << "[Init] -> Created output/weights.bin\n";
    }

    std::cout << "\n[Success] All binary databases initialized and synchronized!\n";
    return 0;
}