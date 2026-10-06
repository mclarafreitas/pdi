#include <iostream>
#include <opencv2/opencv.hpp>
#include "camera.hpp"

int main(int argc, char** argv) {

    cv::Mat image;
    cv::Mat gray;
    cv::Mat equalized;

    int width, height;
    int camera;
    int key;

    cv::VideoCapture cap;

    // Seleciona a câmera disponível
    camera = cameraEnumerator();
    cap.open(camera);

    if (!cap.isOpened()) {
        std::cout << "Cameras indisponiveis" << std::endl;
        return -1;
    }

    // Define a resolução
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);

    std::cout << "largura = " << width << std::endl;
    std::cout << "altura  = " << height << std::endl;

    while (1) {

        // Captura um quadro
        cap >> image;

        if (image.empty()) {
            std::cout << "Erro ao capturar imagem." << std::endl;
            break;
        }

        // Converte para tons de cinza
        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

        // Equaliza o histograma
        cv::equalizeHist(gray, equalized);

        // Exibe a imagem equalizada
        cv::imshow("Imagem Equalizada", equalized);

        // ESC encerra
        key = cv::waitKey(30);

        if (key == 27)
            break;
    }

    cap.release();
    cv::destroyAllWindows();

    return 0;
}