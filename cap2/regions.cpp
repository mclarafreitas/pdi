#include <iostream>
#include <opencv2/opencv.hpp>

int main(int, char**) {

    cv::Mat image;

    // Abre a imagem em tons de cinza
    image = cv::imread("bolhas.png", cv::IMREAD_GRAYSCALE);

    if (!image.data) {
        std::cout << "Nao abriu bolhas.png" << std::endl;
        return -1;
    }

    // Dimensoes da imagem
    int largura = image.cols;
    int altura = image.rows;

    std::cout << "Tamanho da imagem: "
              << largura << " x " << altura << std::endl;

    // Coordenadas dos dois pontos
    int x1, y1, x2, y2;

    std::cout << "Digite as coordenadas do primeiro ponto (x y): ";
    std::cin >> x1 >> y1;

    std::cout << "Digite as coordenadas do segundo ponto (x y): ";
    std::cin >> x2 >> y2;

    // Verifica se as coordenadas estao dentro da imagem
    if (x1 < 0 || x1 >= largura ||
        x2 < 0 || x2 >= largura ||
        y1 < 0 || y1 >= altura ||
        y2 < 0 || y2 >= altura) {

        std::cout << "Coordenadas fora dos limites da imagem!"
                  << std::endl;

        return -1;
    }

    // Garante que x1 <= x2 e y1 <= y2
    int xmin = std::min(x1, x2);
    int xmax = std::max(x1, x2);
    int ymin = std::min(y1, y2);
    int ymax = std::max(y1, y2);

    // Inverte os pixels dentro da regiao
    for (int y = ymin; y <= ymax; y++) {
        for (int x = xmin; x <= xmax; x++) {

            image.at<uchar>(y, x) =
                255 - image.at<uchar>(y, x);
        }
    }

    // Exibe a imagem
    cv::namedWindow("janela", cv::WINDOW_AUTOSIZE);
    cv::imshow("janela", image);
    cv::waitKey();

    return 0;
}