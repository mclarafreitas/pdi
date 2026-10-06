#include <iostream>
#include <opencv2/opencv.hpp>

int main(int, char**) {

    cv::Mat image;

    // Carrega a imagem em escala de cinza
    image = cv::imread("bolhas.png", cv::IMREAD_GRAYSCALE);

    if (!image.data) {
        std::cout << "Nao abriu bolhas.png" << std::endl;
        return -1;
    }

    // Dimensoes da imagem
    int largura = image.cols;
    int altura = image.rows;

    // Como a imagem possui dimensoes multiplas de 2,
    // podemos dividir largura e altura pela metade.
    int metadeLargura = largura / 2;
    int metadeAltura = altura / 2;

    // Cria os quatro quadrantes da imagem usando cv::Mat e cv::Rect
    cv::Mat superiorEsquerdo(
        image,
        cv::Rect(0, 0, metadeLargura, metadeAltura)
    );

    cv::Mat superiorDireito(
        image,
        cv::Rect(metadeLargura, 0, metadeLargura, metadeAltura)
    );

    cv::Mat inferiorEsquerdo(
        image,
        cv::Rect(0, metadeAltura, metadeLargura, metadeAltura)
    );

    cv::Mat inferiorDireito(
        image,
        cv::Rect(metadeLargura, metadeAltura,
                 metadeLargura, metadeAltura)
    );

    // Guarda temporariamente os quadrantes antes da troca
    cv::Mat tempSuperiorEsquerdo = superiorEsquerdo.clone();
    cv::Mat tempSuperiorDireito = superiorDireito.clone();

    // Troca os quadrantes na diagonal
    inferiorDireito.copyTo(superiorEsquerdo);
    tempSuperiorEsquerdo.copyTo(inferiorDireito);

    inferiorEsquerdo.copyTo(superiorDireito);
    tempSuperiorDireito.copyTo(inferiorEsquerdo);

    // Exibe a imagem modificada
    cv::namedWindow("janela", cv::WINDOW_AUTOSIZE);
    cv::imshow("janela", image);
    cv::waitKey();

    return 0;
}