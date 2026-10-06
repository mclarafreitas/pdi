#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

int main(int argc, char** argv) {

    cv::Mat image;
    cv::Mat hsv;
    cv::Mat h, s, v;

    std::vector<cv::Mat> planes;

    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <Image_Path>\n";
        return -1;
    }

    image = cv::imread(argv[1], cv::IMREAD_COLOR);

    if (!image.data) {
        std::cerr << "Nao abriu " << argv[1] << std::endl;
        return -1;
    }

    // Exibe imagem original
    cv::namedWindow("Imagem original", cv::WINDOW_NORMAL);
    cv::imshow("Imagem original", image);

    // Converte para HSV
    cv::cvtColor(image, image, cv::COLOR_BGR2HSV_FULL);

    // Separa os canais H, S e V
    cv::split(image, planes);

    h = planes[0];
    s = planes[1];
    v = planes[2];

    // Aplica 5 colormaps diferentes ao canal H

    cv::Mat jet;
    cv::applyColorMap(h, jet, cv::COLORMAP_JET);
    cv::imshow("JET", jet);

    cv::Mat ocean;
    cv::applyColorMap(h, ocean, cv::COLORMAP_OCEAN);
    cv::imshow("OCEAN", ocean);

    cv::Mat pink;
    cv::applyColorMap(h, pink, cv::COLORMAP_PINK);
    cv::imshow("PINK", pink);

    cv::Mat twilight;
    cv::applyColorMap(h, twilight, cv::COLORMAP_TWILIGHT);
    cv::imshow("TWILIGHT", twilight);

    cv::Mat turbo;
    cv::applyColorMap(h, turbo, cv::COLORMAP_TURBO);
    cv::imshow("TURBO", turbo);

    cv::waitKey();

    return 0;
}