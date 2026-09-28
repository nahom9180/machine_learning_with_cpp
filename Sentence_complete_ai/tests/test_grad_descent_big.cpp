#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>
#include <iostream>

int main(){
    Eigen::Matrix<float, Eigen::Dynamic, 2> inputs(6,2);
    Eigen::Matrix<float, Eigen::Dynamic, 1> outputs(6,1);

    inputs << 1.0f, 1.0f,
    1.0f, 2.0f,
    1.0f, 4.0f,
    1.0f, 8.0f,
    1.0f, 16.0f,
    1.0f, 32.0f;

    outputs << 3.0f, 5.0f,
    9.0f, 17.0f,
    33.0f, 65.0f;

    // now calculating the regression wieghts and balance

    Eigen::Matrix<float, Eigen::Dynamic, 1> Wb(2,1);

    Wb << 0.0f, 0.0f;
    // now Wb = inversed(inputs)*outputs
    // but inputs is not sqaure matrix so it is noninvertible
    // so use inverse(transpose*not_transposed)*transposed * outputs
    // should be easy! well not for the cpu LOL

    float learning_rate = 0.0005f;
    int epochs = 50000;

    for (int i = 0; i < epochs; ++i){
        // lets do a best guess we have
        auto y_hat = inputs * Wb;
        auto error = y_hat - outputs;// a vector pointing from wrong guess to the correct guess

        auto g_descent = (2.0f/6.0f) * inputs.transpose() * error;
        std::cout<<"Epoch: "<<i<<" Slope: "<<Wb(0)<<" Bias: "<<Wb(1)<<"\n";
        Wb = Wb - (learning_rate * g_descent);
    }

    std::cout<<Wb;
}
