#include <iostream>
#include <opencv2/opencv.hpp>
#include "camera.hpp"

void printmask(cv::Mat &m) {
    for (int i = 0; i < m.rows; i++) {
        for (int j = 0; j < m.cols; j++) {
            std::cout << m.at<float>(i, j) << " ";
        }
        std::cout << std::endl;
    }
}

// Cria uma máscara do filtro da média de tamanho n x n
cv::Mat criaMascaraMedia(int n) {
    cv::Mat mask(n, n, CV_32F);

    float valor = 1.0f / (n * n);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            mask.at<float>(i, j) = valor;
        }
    }

    return mask;
}

int main(int argc, char **argv) {

    cv::VideoCapture cap;
    int camera;

    // Tamanhos das máscaras que serão comparados
    int tamanho1 = 3;
    int tamanho2 = 5;

    // Permite informar os tamanhos pela linha de comando
    if (argc == 3) {
        tamanho1 = atoi(argv[1]);
        tamanho2 = atoi(argv[2]);
    }

    // Os tamanhos precisam ser ímpares
    if (tamanho1 % 2 == 0 || tamanho2 % 2 == 0) {
        std::cerr << "Os tamanhos das mascaras devem ser impares." << std::endl;
        return -1;
    }

    cv::Mat frame;
    cv::Mat framegray;
    cv::Mat frame32f;

    cv::Mat mask1;
    cv::Mat mask2;

    cv::Mat result1;
    cv::Mat result2;

    cv::Mat filtered1;
    cv::Mat filtered2;

    char key;

    // Cria as máscaras
    mask1 = criaMascaraMedia(tamanho1);
    mask2 = criaMascaraMedia(tamanho2);

    std::cout << "\nMascara " << tamanho1 << "x" << tamanho1 << ":\n";
    printmask(mask1);

    std::cout << "\nMascara " << tamanho2 << "x" << tamanho2 << ":\n";
    printmask(mask2);

    // Inicializa a câmera
    camera = cameraEnumerator();
    cap.open(camera);

    if (!cap.isOpened()) {
        std::cerr << "Erro ao abrir a camera." << std::endl;
        return -1;
    }

    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    cv::namedWindow("original", cv::WINDOW_NORMAL);
    cv::namedWindow("media " + std::to_string(tamanho1) + "x" +
                    std::to_string(tamanho1), cv::WINDOW_NORMAL);
    cv::namedWindow("media " + std::to_string(tamanho2) + "x" +
                    std::to_string(tamanho2), cv::WINDOW_NORMAL);

    for (;;) {

        // Captura uma imagem
        cap >> frame;

        if (frame.empty())
            break;

        // Converte para tons de cinza
        cv::cvtColor(frame, framegray, cv::COLOR_BGR2GRAY);

        // Espelha a imagem
        cv::flip(framegray, framegray, 1);

        cv::imshow("original", framegray);

        // Converte para float
        framegray.convertTo(frame32f, CV_32F);

        // Convolução com a primeira máscara
        cv::filter2D(
            frame32f,
            filtered1,
            frame32f.depth(),
            mask1,
            cv::Point(tamanho1 / 2, tamanho1 / 2),
            cv::BORDER_REPLICATE
        );

        // Convolução com a segunda máscara
        cv::filter2D(
            frame32f,
            filtered2,
            frame32f.depth(),
            mask2,
            cv::Point(tamanho2 / 2, tamanho2 / 2),
            cv::BORDER_REPLICATE
        );

        // Converte para 8 bits para exibição
        filtered1.convertTo(result1, CV_8U);
        filtered2.convertTo(result2, CV_8U);

        // Mostra os resultados
        cv::imshow(
            "media " + std::to_string(tamanho1) + "x" +
            std::to_string(tamanho1),
            result1
        );

        cv::imshow(
            "media " + std::to_string(tamanho2) + "x" +
            std::to_string(tamanho2),
            result2
        );

        // ESC encerra
        key = (char)cv::waitKey(10);

        if (key == 27)
            break;
    }

    return 0;
}