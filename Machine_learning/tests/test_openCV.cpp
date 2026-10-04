#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    cv::Mat img = cv::imread("tests/data/test.jpg"); // this loads the image as an array of colors in BGR! it saves us of unpacking the jpeg compression

    if (img.empty()) {
        std::cerr << "Failed to load image! Did you put test.jpg in tests/data/ ?\n";
        return 1;
    }

    std::cout << "Original Image:\n";
    std::cout << "Height: " << img.rows << "\n";
    std::cout << "Width:  " << img.cols << "\n";
    std::cout << "Channels: " << img.channels() << "\n\n";

    cv::cvtColor(img, img, cv::COLOR_BGR2RGB);//this revamps the image to have format of RGB the colors remain but suitable for modern

    cv::resize(img, img, cv::Size(224, 224));// squashed

    std::cout << "Modified Image:\n";
    std::cout << "Height: " << img.rows << "\n";
    std::cout << "Width:  " << img.cols << "\n";
    std::cout << "Channels: " << img.channels() << "\n";

    cv::imwrite("tests/data/output.jpg", img);

    return 0;
}
