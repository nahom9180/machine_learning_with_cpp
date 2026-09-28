#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>
#include <iostream>

int main(){
    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor> m(1,3);
    Eigen::Matrix<float, Eigen::Dynamic,Eigen::Dynamic, Eigen::RowMajor > n(3,1);

    m << 3.0f, 4.0f, 5.0f;
        //5.0f, 4.0f, 23.0f,
        //4.0f, 1.0f, 23.0f;  //this is to feed the thing peice by peice as stream?

    n << 3.0f, 4.0f, 5.0f;
       // 9.0f, 20.0f, 43.0f,
       // 5.0f, 5.0f, 3.0f;

    std::cout <<m<<"\n dot \n"<<n
        <<"\n = \n"<< m*n;
    return 0;
}
