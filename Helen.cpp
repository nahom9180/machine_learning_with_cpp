#include <array>
#include <iostream>
#include <cmath>
#include "config.hpp" // <--- Include single config
#include <array>
#include <cmath>

const int LAYERS = CFG_LAYERS;
const int TOKENS = CFG_TOKENS;
const int DIMENSIONS = CFG_DIMENSIONS;

using Matrix2d = std::array<std::array<float, DIMENSIONS>, DIMENSIONS>;
using Matrix3d = std::array<std::array<std::array<float, DIMENSIONS>, TOKENS>, LAYERS>;
using Vector   = std::array<float, DIMENSIONS>;

class KnowledgeClusterCpp {
    private:
        Matrix3d ltd = {};
        // l-layer t-token_id d-dimension
        Matrix2d weights = {};
        // {} is very important! it initializes them with 0.0f for all 100

    public:
        KnowledgeClusterCpp(){
            // constructor
        }

        // getters
        float get_weught(int row, int col) const{
            return weights[row][col];
        }

        float get_token_dim(int layer, int token_id, int dim) const{
            return ltd[layer][token_id][dim];
        }

        Vector get_token(int layer, int token_id) const{
            return ltd[layer][token_id];
        }

        // setters
        void set_token(int layer, int token_id, const Vector& new_vector) {
            ltd[layer][token_id] = new_vector;
        }
        void set_token_dim(int layer, int token_id, int dim,float value){
            ltd[layer][token_id][dim] = value;
        }

        // operations

        void do_scale_token(int layer, int token_id, float factor){
            for (int d = 0; d < DIMENSIONS; d++){
            ltd[layer][token_id][d] *= factor;
            }
        }

        void do_add_to_token(int layer, int token_id, const Vector& delta_vector){
            for (int d=0; d < DIMENSIONS; d++){
                ltd[layer][token_id][d] += delta_vector[d];
            }
        }
        void do_normalize_token(int layer, int token_id){
            float squared_length = 0;
            for (int d=0; d < DIMENSIONS; d++){
                float val = ltd[layer][token_id][d];
                squared_length += val * val;
            }

            float length = sqrt(squared_length);

            for (int d=0; d < 10; d++){
                ltd[layer][token_id][d] /= length;
            }
        }

        // inferences
        float compute_dotproduct(int layer, int token_id, const Vector& input_vec) const{
            float result = 0.0f;
            Vector token_vec = ltd[layer][token_id];
            for (int d = 0; d < DIMENSIONS; d++){
                result += input_vec[d] * token_vec[d];
            }

            return result;
        }

        int find_best_matching(int layer, const Vector& output_vec) const{
            // returns the token_id of the best matching

            int best_token_id = 0;
            float highest_score = -1e9f;
            for (int t = 0; t < TOKENS; t++){
                float score = compute_dotproduct(layer, t, output_vec);
                if (score > highest_score){ 
                    highest_score = score;
                    best_token_id = t;
                }
            }
            return best_token_id;
        }
};

