#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <random>
#include <filesystem>
#include <iomanip>
#include "core/config.hpp"
#include "core/embedding_store.hpp"

int main() {
    std::filesystem::create_directories("output");

    // 1. READ RAW DATASET
    std::string data_path = "data/dataset.txt";
    std::ifstream dataset(data_path);
    if (!dataset.is_open()) {
        dataset.open("dataset.txt");
    }

    if (!dataset.is_open()) {
        std::cerr << "Error: Could not open dataset at data/dataset.txt or dataset.txt\n";
        return 1;
    }

    std::unordered_map<std::string, int> counts;
    std::string word;
    while (dataset >> word) {
        counts[word]++;
    }
    dataset.close();

    // Sort descending by frequency
    std::vector<std::pair<std::string, int>> freq_list(counts.begin(), counts.end());
    std::sort(freq_list.begin(), freq_list.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    size_t actual_tokens = std::min(freq_list.size(), CFG_TOKENS);

    // 2. WRITE `output/tokens.bin`
    size_t token_bytes_written = 0;
    {
        std::ofstream token_file("output/tokens.bin", std::ios::binary);
        uint32_t count = static_cast<uint32_t>(actual_tokens);
        token_file.write(reinterpret_cast<const char*>(&count), sizeof(count));
        token_bytes_written += sizeof(count);

        for (size_t i = 0; i < actual_tokens; ++i) {
            const std::string& str = freq_list[i].first;
            uint16_t len = static_cast<uint16_t>(str.size());
            token_file.write(reinterpret_cast<const char*>(&len), sizeof(len));
            token_file.write(str.data(), len);
            token_bytes_written += sizeof(len) + len;
        }
    }

    // 3. WRITE `output/embeddings.bin`
    size_t embed_elements = actual_tokens * CFG_DIMENSIONS;
    size_t embed_bytes = sizeof(ModelHeader) + (embed_elements * sizeof(float));
    {
        std::ofstream embed_file("output/embeddings.bin", std::ios::binary);
        ModelHeader header;
        header.vocab_size = static_cast<uint32_t>(actual_tokens);
        header.dim = static_cast<uint32_t>(CFG_DIMENSIONS);
        embed_file.write(reinterpret_cast<const char*>(&header), sizeof(ModelHeader));

        std::mt19937 rng(42);
        std::uniform_real_distribution<float> dist(-0.1f, 0.1f);

        std::vector<float> embeds(embed_elements);
        for (float& val : embeds) {
            val = dist(rng);
        }

        embed_file.write(reinterpret_cast<const char*>(embeds.data()), embeds.size() * sizeof(float));
    }

    // 4. WRITE `output/weights.bin`
    size_t m2_elements = CFG_DIMENSIONS * CFG_DIMENSIONS;
    size_t m3_elements = CFG_LAYERS * actual_tokens * CFG_DIMENSIONS;
    size_t total_weight_elements = m2_elements + m3_elements;
    size_t weight_bytes = total_weight_elements * sizeof(float);
    {
        std::ofstream weight_file("output/weights.bin", std::ios::binary);
        std::vector<float> zeros(total_weight_elements, 0.0f);
        weight_file.write(reinterpret_cast<const char*>(zeros.data()), zeros.size() * sizeof(float));
    }

    // ==========================================================
    // DETAILED SUMMARY REPORT
    // ==========================================================
    std::cout << "\n============================================================\n";
    std::cout << "               HELEN MODEL INITIALIZATION REPORT            \n";
    std::cout << "============================================================\n";

    std::cout << std::fixed << std::setprecision(2);

    // 1. Vocabulary info
    std::cout << "[Tokens Database]\n";
    std::cout << "  • Unique words in text : " << counts.size() << "\n";
    std::cout << "  • Tokens created       : " << actual_tokens << " words\n";
    std::cout << "  • File path            : output/tokens.bin (" 
              << (token_bytes_written / 1024.0) << " KB)\n\n";

    // 2. Embedding Table info
    std::cout << "[Embedding Matrix]\n";
    std::cout << "  • Dimensions per token : " << CFG_DIMENSIONS << " float32\n";
    std::cout << "  • Matrix shape         : [" << actual_tokens << " x " << CFG_DIMENSIONS << "]\n";
    std::cout << "  • Total parameters     : " << embed_elements << " floats\n";
    std::cout << "  • File path            : output/embeddings.bin (" 
              << (embed_bytes / 1024.0) << " KB)\n\n";

    // 3. Weight Matrices info
    std::cout << "[Weight Matrices (Knowledge Cluster)]\n";
    std::cout << "  • 2D Projection Weights: [" << CFG_DIMENSIONS << " x " << CFG_DIMENSIONS 
              << "] = " << m2_elements << " floats (" 
              << (m2_elements * sizeof(float) / 1024.0) << " KB)\n";

    std::cout << "  • 3D Layer Weights     : [" << CFG_LAYERS << " layers x " 
              << actual_tokens << " tokens x " << CFG_DIMENSIONS << " dims] = " 
              << m3_elements << " floats (" 
              << (m3_elements * sizeof(float) / 1024.0) << " KB)\n";

    std::cout << "  • Total cluster weights: " << total_weight_elements << " floats\n";
    std::cout << "  • File path            : output/weights.bin (" 
              << (weight_bytes / 1024.0) << " KB)\n";

    std::cout << "============================================================\n";
    std::cout << "✔ All databases initialized and synchronized successfully!\n\n";

    return 0;
}