// this code shows the other power of Eigen.
// instead of reading each floats in a CSV file and loading them and constructing a Matrix
// this uses the csv x by y entire matrix and pretends it is an Eigen matrix object right their still in the SSD

#include "../csv.h"
#include <vector>
#include <iostream>
#include <eigen3/Eigen/Dense>

int main() {
    io::CSVReader<3> in("tests/data/sample.csv"); // 3 represents the number of columns expected

    float c1, c2, c3;
    std::vector<float> data_bucket;

    while(in.read_row(c1, c2, c3)) { // it may be like read_row puts the read results to these
        data_bucket.push_back(c1);
        data_bucket.push_back(c2);
        data_bucket.push_back(c3);
    }// whats pushback i dont know what this does it may be a resizable array that keeps on increasing its size.
    //  now we know Eigens put data as a in a ram so if we initialize it using the data then telling it the colums and rows will building the existing
    //  is this mehod >> only available on Eigen::Map. cool!

    Eigen::Map<Eigen::Matrix<float, 3, 3, Eigen::RowMajor>> mapped_matrix(data_bucket.data());
    // We set it to row major because the csv is also doing that.
    std::cout << "Successfully loaded CSV into Eigen Matrix:\n";
    std::cout << mapped_matrix << "\n";

    return 0;
}
