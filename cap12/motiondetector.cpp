#include <iostream>
#include <opencv2/opencv.hpp>

int main(int argc, char** argv) {

    cv::Mat image;
    cv::Mat gray;

    cv::Mat histAtual;
    cv::Mat histAnterior;

    // Número de bins do histograma
    int nbins = 64;

    float range[] = {0, 256};
    const float* histrange = {range};

    bool uniform = true;
    bool accumulate = false;

    // Limiar para ativar o alarme
    double limiar = 0.30;

    // Abre a câmera
    cv::VideoCapture cap(0);

    if (!cap.isOpened()) {
        std::cout << "Nao foi possivel abrir a camera." << std::endl;
        return -1;
    }

    // Define a resolução
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    std::cout << "Detector de movimento iniciado." << std::endl;
    std::cout << "Limiar = " << limiar << std::endl;
    std::cout << "Pressione ESC para sair." << std::endl;

    bool primeiroFrame = true;

    while (true) {

        // Captura o frame
        cap >> image;

        if (image.empty()) {
            std::cout << "Erro ao capturar imagem." << std::endl;
            break;
        }

        // Converte para tons de cinza
        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

        // Calcula o histograma do frame atual
        cv::calcHist(
            &gray,
            1,
            0,
            cv::Mat(),
            histAtual,
            1,
            &nbins,
            &histrange,
            uniform,
            accumulate
        );

        // Normaliza o histograma
        cv::normalize(
            histAtual,
            histAtual,
            0,
            1,
            cv::NORM_MINMAX
        );

        // No primeiro frame não existe histograma anterior
        if (primeiroFrame) {

            histAtual.copyTo(histAnterior);
            primeiroFrame = false;

            cv::putText(
                image,
                "Inicializando...",
                cv::Point(20, 40),
                cv::FONT_HERSHEY_SIMPLEX,
                1.0,
                cv::Scalar(0, 255, 255),
                2
            );

        } else {

            // Compara o histograma atual com o anterior
            double diferenca = cv::compareHist(
                histAtual,
                histAnterior,
                cv::HISTCMP_BHATTACHARYYA
            );

            std::cout << "Diferenca: " << diferenca;

            // Verifica se ultrapassou o limiar
            if (diferenca > limiar) {

                std::cout << " -> MOVIMENTO DETECTADO!" << std::endl;

                // Alarme visual
                cv::putText(
                    image,
                    "!!! MOVIMENTO DETECTADO !!!",
                    cv::Point(20, 40),
                    cv::FONT_HERSHEY_SIMPLEX,
                    1.0,
                    cv::Scalar(0, 0, 255),
                    3
                );

                // Borda vermelha na imagem
                cv::rectangle(
                    image,
                    cv::Point(0, 0),
                    cv::Point(image.cols - 1, image.rows - 1),
                    cv::Scalar(0, 0, 255),
                    10
                );

            } else {

                std::cout << " -> Sem movimento" << std::endl;

                cv::putText(
                    image,
                    "Sem movimento",
                    cv::Point(20, 40),
                    cv::FONT_HERSHEY_SIMPLEX,
                    1.0,
                    cv::Scalar(0, 255, 0),
                    2
                );
            }

            // O histograma atual passa a ser o anterior
            histAtual.copyTo(histAnterior);
        }

        // Mostra a imagem
        cv::imshow("Motion Detector", image);

        // ESC para sair
        int key = cv::waitKey(30);

        if (key == 27)
            break;
    }

    cap.release();
    cv::destroyAllWindows();

    return 0;
}