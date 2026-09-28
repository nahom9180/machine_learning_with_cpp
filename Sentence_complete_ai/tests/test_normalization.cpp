#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>
#include <iostream>
#include "../csv.h"

int main(){
    Eigen::VectorXf x(10);
    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> unrefined(20, 7);

    x << 5.0f, 4.0f,
    1.0f, 4.0f,
    1.0f, 8.0f,
    1.0f, 16.0f,
    1.0f, 32.0f;

    float min = x.minCoeff();
    float max = x.maxCoeff();

    Eigen::VectorXf normalized(10);

    normalized.array() = (x.array() - min)/(max - min);
    // now calculating the regression wieghts and balance
    std::cout<< x <<"\nNormalized\n" <<normalized;
}
