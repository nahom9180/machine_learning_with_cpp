#pragma once

#include "config.hpp"
#include "math.hpp"
#include <array>
#include <fstream>
#include <filesystem>
#include <string>

constexpr size_t LAYERS     = CFG_LAYERS;
constexpr size_t TOKENS     = CFG_TOKENS;
constexpr size_t DIMENSIONS = CFG_DIMENSIONS;

using Vector   = HelenMath::Vector;
using Matrix2d = HelenMath::Matrix2d;
using Matrix3d = std::array<std::array<std::array<float, DIMENSIONS>, TOKENS>, LAYERS>;

class KnowledgeClusterCpp {
private:
    Matrix2d weights = {}; // 2D Projection Weights [D x D]
    Matrix3d ltd     = {}; // 3D Layer Cluster Memory [L x T x D]

public:
    KnowledgeClusterCpp() = default;

    // Getters & Setters
    float get_weight(size_t r, size_t c) const { return weights[r][c]; }
    void  set_weight(size_t r, size_t c, float v) { weights[r][c] = v; }

    Vector get_token(size_t l, size_t t) const { return ltd[l][t]; }
    void   set_token(size_t l, size_t t, const Vector& v) { ltd[l][t] = v; }

    // --- Math Delegated to HelenMath ---
    Vector transform(const Vector& x) const {
        return HelenMath::mat_vec_mul(weights, x);
    }

    Vector transform_transpose(const Vector& y) const {
        return HelenMath::mat_vec_mul_transpose(weights, y);
    }

    void do_outer_product_update(const Vector& u, const Vector& v, float lr) {
        HelenMath::outer_product_add(weights, u, v, lr);
    }

    // --- Cluster Mutation for Training ---
    void update_cluster_token(size_t layer, size_t token_id, const Vector& delta, float lr) {
        Vector current = ltd[layer][token_id];
        Vector update = HelenMath::scale(delta, lr);
        ltd[layer][token_id] = HelenMath::add(current, update);
    }

    void do_normalize_token(size_t layer, size_t token_id) {
        ltd[layer][token_id] = HelenMath::normalize(ltd[layer][token_id]);
    }

    float compute_dotproduct(size_t layer, size_t token_id, const Vector& input_vec) const {
        return HelenMath::dot(ltd[layer][token_id], input_vec);
    }

    int find_best_matching(size_t layer, const Vector& output_vec) const {
        int best_token_id = 0;
        float highest_score = -1e9f;
        for (size_t t = 0; t < TOKENS; ++t) {
            float score = compute_dotproduct(layer, t, output_vec);
            if (score > highest_score) {
                highest_score = score;
                best_token_id = static_cast<int>(t);
            }
        }
        return best_token_id;
    }

    // Persistence
    bool load_from_file(const std::string& filepath) {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open()) return false;
        file.read(reinterpret_cast<char*>(weights.data()), sizeof(weights));
        file.read(reinterpret_cast<char*>(ltd.data()), sizeof(ltd));
        return file.good();
    }

    bool save_to_file(const std::string& filepath) const {
        std::string temp = filepath + ".tmp";
        std::ofstream file(temp, std::ios::binary | std::ios::trunc);
        if (!file.is_open()) return false;
        file.write(reinterpret_cast<const char*>(weights.data()), sizeof(weights));
        file.write(reinterpret_cast<const char*>(ltd.data()), sizeof(ltd));
        file.close();
        std::error_code ec;
        std::filesystem::rename(temp, filepath, ec);
        return !ec;
    }
};