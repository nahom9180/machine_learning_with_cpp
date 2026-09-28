#include <iostream>
#include <chrono>
#include <eigen3/Eigen/Dense>

int main() {
    const int rows = 5000;
    const int cols = 5000;


    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor> m(rows, cols);
    // the colmajor only and only affects how the matrix is stored in the memory nothing else!
    // accesing the matrix via eigen wrapper will not be changed at all
    // we have only one matrix! ok we are just checking SIMD on itself

    // a float is 32bit or 4bytes so 4bytes * 20k * 20k = 1,600,000,000bytes = 1.6GB
    m.setConstant(1.0f); // this make the entire to be 1.0f

    auto start_a = std::chrono::high_resolution_clock::now(); //clock starts here
    volatile float sum_a = 0.0f; // we see this sum is never used anywhere so cpu is like nah why bother!! but if we say volatile it will do anyways
    for (int col = 0; col < cols; ++col) {
        //
        for (int row = 0; row < rows; ++row) {
            // the entire matrix is summed up or like the summer sweeps accroos the entire 5k by 5k matrix totalling 25000000 floats!)
            // now if SIMD worked it should be over 8times faster right?!!
            // since the matrix is colmajor the higher loop will be jumpy as the matrixmemory is also jumpy for columns
            // this should be good for SIMD
            sum_a += m(row, col);
        }
    }
    auto end_a = std::chrono::high_resolution_clock::now();// clock ends the moment the loop ends
    std::chrono::duration<double, std::milli> time_a = end_a - start_a;

    auto start_b = std::chrono::high_resolution_clock::now();
    volatile float sum_b = 0.0f;
    for (int row = 0; row < rows; ++row) {
        // as the higher loop is rows and our rows are contigous but the inner loop is columns these change super fast but are jumpy so there is a lot of jumping and cach misses!!
        // this should be bad for SIMD
        for (int col = 0; col < cols; ++col) {
            sum_b += m(row, col);
        }
    }
    auto end_b = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> time_b = end_b - start_b;

    std::cout << "Test A (Sequential / Cache-friendly): " << time_a.count() << " ms\n";
    std::cout << "Test B (Strided / Cache-unfriendly):   " << time_b.count() << " ms\n";
    std::cout << "Slowdown Factor: " << (time_b.count() / time_a.count()) << "x\n";

    return 0;
}
