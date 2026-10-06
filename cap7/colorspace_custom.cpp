#include <iostream>
#include <opencv2/opencv.hpp>

int main(int argc, char** argv) {

    cv::Mat image, imageColor;

    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <Image_Path>\n";
        return -1;
    }

    // Abre a imagem em tons de cinza
    image = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);

    if (image.empty()) {
        std::cerr << "Nao abriu " << argv[1] << std::endl;
        return -1;
    }

    // Cria uma LUT com 256 cores
    // 256 linhas x 1 coluna, 3 canais (BGR)
    cv::Mat colormap(256, 1, CV_8UC3);

    for (int i = 0; i < 256; i++) {

        uchar B, G, R;

        if (i < 64) {
            // Azul escuro -> Azul
            B = 80 + i * 175 / 63;
            G = 0;
            R = 0;
        }
        else if (i < 128) {
            // Azul -> Roxo
            B = 255;
            G = 0;
            R = (i - 64) * 180 / 63;
        }
        else if (i < 192) {
            // Roxo -> Vermelho
            B = 255 - (i - 128) * 255 / 63;
            G = 0;
            R = 180 + (i - 128) * 75 / 63;
        }
        else {
            // Vermelho -> Amarelo
            B = 0;
            G = (i - 192) * 255 / 63;
            R = 255;
        }

        colormap.at<cv::Vec3b>(i, 0) = cv::Vec3b(B, G, R);
    }

    // Aplica o colormap personalizado
    cv::applyColorMap(image, imageColor, colormap);

    // Exibe a imagem original
    cv::namedWindow("Imagem original", cv::WINDOW_NORMAL);
    cv::imshow("Imagem original", image);

    // Exibe a imagem colorida
    cv::namedWindow("Colormap personalizado", cv::WINDOW_NORMAL);
    cv::imshow("Colormap personalizado", imageColor);

    cv::waitKey(0);

    return 0;
}