#include <iostream>
#include <opencv2/opencv.hpp>

int main(int argc, char** argv) {

    if (argc != 2) {
        std::cout << "Uso: " << argv[0] << " <imagem_esteganografada>" << std::endl;
        return -1;
    }

    cv::Mat imagemEsteganografada;
    cv::Mat imagemRecuperada;

    int nbits = 3;

    imagemEsteganografada = cv::imread(argv[1], cv::IMREAD_COLOR);

    if (imagemEsteganografada.empty()) {
        std::cout << "Imagem nao carregou corretamente" << std::endl;
        return -1;
    }

    imagemRecuperada = cv::Mat::zeros(
        imagemEsteganografada.size(),
        imagemEsteganografada.type()
    );

    for (int i = 0; i < imagemEsteganografada.rows; i++) {
        for (int j = 0; j < imagemEsteganografada.cols; j++) {

            cv::Vec3b pixel = imagemEsteganografada.at<cv::Vec3b>(i, j);
            cv::Vec3b recuperado;

            // Pega os 3 bits menos significativos
            // e coloca nos 3 bits mais significativos.
            recuperado[0] = (pixel[0] & 0x07) << (8 - nbits);
            recuperado[1] = (pixel[1] & 0x07) << (8 - nbits);
            recuperado[2] = (pixel[2] & 0x07) << (8 - nbits);

            imagemRecuperada.at<cv::Vec3b>(i, j) = recuperado;
        }
    }

    cv::imwrite("imagem-recuperada.png", imagemRecuperada);

    std::cout << "Imagem recuperada salva como: imagem-recuperada.png"
              << std::endl;

    return 0;
}