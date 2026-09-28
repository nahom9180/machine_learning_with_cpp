#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>
#include <iostream>
#include <stdio.h>

int main(){
    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor> m(3,3);
    Eigen::Matrix<int, Eigen::Dynamic,Eigen::Dynamic, Eigen::RowMajor > n(3,3);

    m << 3.0f, 4.0f, 5.0f,
        5.0f, 4.0f, 23.0f,
        4.0f, 1.0f, 23.0f;  //this is to feed the thing peice by peice as stream?

    n << 3, 4, 5,
        9, 20, 43,
        5, 5, 3;

    std::cout<< "Matrix A\n" <<
        m <<"\n Matrix B\n"<<n;
    // there is a memory location to print data to the terminal and that is signified by cout; so if we feed it it just prints out automatically
    float* raw_mem = m.data();

    for (int i = 0; i < 10; ++i){
        std::cout<< "\nMindex: "<<i<<" Data: "<< raw_mem[i]<<"\n";//this also is feeding
    }

    int* raw_mem2 = n.data();

    for (int i=0; i<9;++i){
        std::cout<< "\nMindex: "<<i<<" Data: "<< raw_mem2[i]<<"\n";
    }
    return 0;
}
