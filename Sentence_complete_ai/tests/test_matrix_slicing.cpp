#include <eigen3/Eigen/Dense>
#include <iostream>

int main(){
    Eigen::Matrix<float, 4, 4> m(4,4);
    m.setOnes();
    m.block(0,0,2,2) *= 2.0f;
    m.block(0,2,2,2) *= 2.0f;
    m.block(2,0,2,2) *= 2.0f;
    m.block(2,2,2,2);

    std::cout << m;
    return 0;
}
