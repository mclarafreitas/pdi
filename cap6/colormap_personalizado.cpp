#include <opencv2/opencv.hpp>
#include <iostream>

int main(int argc, const char *argv[]) {

    if (argc < 2) {
        std::cout << "Uso: ./colormap_personalizado imagem.png" << std::endl;
        return -1;
    }

    cv::Mat image, imagecolor;

    // Abre a imagem em tons de cinza
    image = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);

    if (!image.data) {
        std::cout << "Nao abriu a imagem" << std::endl;
        return -1;
    }

    // Cria uma LUT com 256 cores
    cv::Mat lut(256, 1, CV_8UC3);

    // Cria o colormap personalizado
    for (int i = 0; i < 256; i++) {

        if (i < 64) {
            // Azul -> Ciano
            lut.at<cv::Vec3b>(i)[0] = 255;
            lut.at<cv::Vec3b>(i)[1] = i * 4;
            lut.at<cv::Vec3b>(i)[2] = 0;

        } else if (i < 128) {
            // Ciano -> Verde
            lut.at<cv::Vec3b>(i)[0] = 255 - (i - 64) * 4;
            lut.at<cv::Vec3b>(i)[1] = 255;
            lut.at<cv::Vec3b>(i)[2] = 0;

        } else if (i < 192) {
            // Verde -> Amarelo
            lut.at<cv::Vec3b>(i)[0] = 0;
            lut.at<cv::Vec3b>(i)[1] = 255;
            lut.at<cv::Vec3b>(i)[2] = (i - 128) * 4;

        } else {
            // Amarelo -> Vermelho
            lut.at<cv::Vec3b>(i)[0] = 0;
            lut.at<cv::Vec3b>(i)[1] = 255 - (i - 192) * 4;
            lut.at<cv::Vec3b>(i)[2] = 255;
        }
    }

    // Aplica o colormap personalizado
    cv::applyColorMap(image, imagecolor, lut);

    // Exibe o resultado
    cv::imshow("Colormap personalizado", imagecolor);

    cv::waitKey(0);

    return 0;
}