#pragma once

#include "config.hpp"
#include <array>
#include <cmath>

namespace HelenMath {

    using Vector   = std::array<float, CFG_DIMENSIONS>;
    using Matrix2d = std::array<std::array<float, CFG_DIMENSIONS>, CFG_DIMENSIONS>;

    // Vector Addition: a + b
    inline Vector add(const Vector& a, const Vector& b) {
        Vector res = {};
        for (size_t i = 0; i < CFG_DIMENSIONS; ++i) res[i] = a[i] + b[i];
        return res;
    }

    // Vector Subtraction: a - b
    inline Vector sub(const Vector& a, const Vector& b) {
        Vector res = {};
        for (size_t i = 0; i < CFG_DIMENSIONS; ++i) res[i] = a[i] - b[i];
        return res;
    }

    // Scalar Multiplication: a * s
    inline Vector scale(const Vector& a, float s) {
        Vector res = {};
        for (size_t i = 0; i < CFG_DIMENSIONS; ++i) res[i] = a[i] * s;
        return res;
    }

    // Dot Product: <a, b>
    inline float dot(const Vector& a, const Vector& b) {
        float sum = 0.0f;
        for (size_t i = 0; i < CFG_DIMENSIONS; ++i) sum += a[i] * b[i];
        return sum;
    }

    // L2 Vector Norm (Length)
    inline float norm(const Vector& a) {
        return std::sqrt(dot(a, a));
    }

    // Vector Normalization: a / ||a||
    inline Vector normalize(const Vector& a) {
        float len = norm(a);
        if (len < 1e-8f) return a;
        return scale(a, 1.0f / len);
    }

    // Mean Squared Error Loss: 0.5 * ||target - pred||^2
    inline float mse_loss(const Vector& target, const Vector& pred) {
        Vector err = sub(target, pred);
        return 0.5f * dot(err, err);
    }

    // Outer Product Update: W += lr * (u (x) v^T)
    inline void outer_product_add(Matrix2d& W, const Vector& u, const Vector& v, float lr) {
        for (size_t r = 0; r < CFG_DIMENSIONS; ++r) {
            for (size_t c = 0; c < CFG_DIMENSIONS; ++c) {
                W[r][c] += lr * u[r] * v[c];
            }
        }
    }

    // Matrix * Vector Multiplication: y = W * x
    inline Vector mat_vec_mul(const Matrix2d& W, const Vector& x) {
        Vector y = {};
        for (size_t r = 0; r < CFG_DIMENSIONS; ++r) {
            float sum = 0.0f;
            for (size_t c = 0; c < CFG_DIMENSIONS; ++c) {
                sum += W[r][c] * x[c];
            }
            y[r] = sum;
        }
        return y;
    }

    // Transpose Matrix * Vector: x = W^T * y
    inline Vector mat_vec_mul_transpose(const Matrix2d& W, const Vector& y) {
        Vector x = {};
        for (size_t c = 0; c < CFG_DIMENSIONS; ++c) {
            float sum = 0.0f;
            for (size_t r = 0; r < CFG_DIMENSIONS; ++r) {
                sum += W[r][c] * y[r];
            }
            x[c] = sum;
        }
        return x;
    }

} // namespace HelenMath