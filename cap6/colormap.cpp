#include <opencv2/opencv.hpp>
#include <iostream>

int main(int argc, const char *argv[]) {

    cv::Mat image, imagecolor;

    image = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);

    if (!image.data) {
        std::cout << "Nao abriu a imagem" << std::endl;
        return -1;
    }

    // AUTUMN
    cv::applyColorMap(image, imagecolor, cv::COLORMAP_AUTUMN);
    cv::imshow("colormap_autumn", imagecolor);
    cv::waitKey(0);

    // BONE
    cv::applyColorMap(image, imagecolor, cv::COLORMAP_BONE);
    cv::imshow("colormap_bone", imagecolor);
    cv::waitKey(0);

    // HOT
    cv::applyColorMap(image, imagecolor, cv::COLORMAP_HOT);
    cv::imshow("colormap_hot", imagecolor);
    cv::waitKey(0);

    // OCEAN
    cv::applyColorMap(image, imagecolor, cv::COLORMAP_OCEAN);
    cv::imshow("colormap_ocean", imagecolor);
    cv::waitKey(0);

    // VIRIDIS
    cv::applyColorMap(image, imagecolor, cv::COLORMAP_VIRIDIS);
    cv::imshow("colormap_viridis", imagecolor);
    cv::waitKey(0);

    return 0;
}