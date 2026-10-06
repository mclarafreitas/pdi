#include <iostream>
#include <opencv2/opencv.hpp>
#include "camera.hpp"

int main(int argc, char **argv) {

    cv::VideoCapture cap;
    int camera;

    // Matriz usada para guardar o maior valor do Laplaciano
    cv::Mat maxLaplacian;

    // Imagens
    cv::Mat frame;
    cv::Mat frameGray;
    cv::Mat frame32f;
    cv::Mat laplacian;
    cv::Mat output;

    // Máscara Laplaciana 3x3
    float lapMaskData[] = {
         0, -1,  0,
        -1,  4, -1,
         0, -1,  0
    };

    cv::Mat lapMask(3, 3, CV_32F, lapMaskData);

    // ---------------------------------------------------------
    // Abre vídeo passado como argumento ou utiliza a câmera
    // ---------------------------------------------------------

    if (argc > 1) {
        cap.open(argv[1]);
    } else {
        camera = cameraEnumerator();
        cap.open(camera);
    }

    if (!cap.isOpened()) {
        std::cerr << "Erro ao abrir a câmera/vídeo." << std::endl;
        return -1;
    }

    // Tamanho desejado para a captura
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    cv::namedWindow("Original", cv::WINDOW_NORMAL);
    cv::namedWindow("Resultado", cv::WINDOW_NORMAL);
    cv::namedWindow("Laplaciano", cv::WINDOW_NORMAL);

    bool primeiroFrame = true;

    while (true) {

        // ---------------------------------------------------------
        // 1. Captura um frame
        // ---------------------------------------------------------

        cap >> frame;

        if (frame.empty())
            break;

        // Se estiver usando câmera, espelha a imagem
        if (argc == 1)
            cv::flip(frame, frame, 1);

        // ---------------------------------------------------------
        // 2. Converte para tons de cinza
        // ---------------------------------------------------------

        cv::cvtColor(frame, frameGray, cv::COLOR_BGR2GRAY);

        // ---------------------------------------------------------
        // 3. Cria a matriz para guardar os máximos
        // ---------------------------------------------------------

        if (primeiroFrame) {
            maxLaplacian = cv::Mat::zeros(
                frameGray.size(),
                CV_32F
            );

            output = frame.clone();

            primeiroFrame = false;
        }

        // ---------------------------------------------------------
        // 4. Aplica o filtro Laplaciano
        // ---------------------------------------------------------

        frameGray.convertTo(frame32f, CV_32F);

        cv::filter2D(
            frame32f,
            laplacian,
            CV_32F,
            lapMask,
            cv::Point(1, 1),
            cv::BORDER_REPLICATE
        );

        // Utilizamos o valor absoluto como medida de nitidez
        cv::Mat laplacianAbs = cv::abs(laplacian);

        // ---------------------------------------------------------
        // 5. Compara com os máximos anteriores
        // ---------------------------------------------------------

        for (int y = 0; y < frame.rows; y++) {

            for (int x = 0; x < frame.cols; x++) {

                float atual = laplacianAbs.at<float>(y, x);
                float maximo = maxLaplacian.at<float>(y, x);

                // Se o pixel atual estiver mais nítido,
                // guarda o novo máximo e o pixel colorido
                if (atual > maximo) {

                    maxLaplacian.at<float>(y, x) = atual;

                    output.at<cv::Vec3b>(y, x) =
                        frame.at<cv::Vec3b>(y, x);
                }
            }
        }

        // ---------------------------------------------------------
        // 6. Exibe a imagem de saída
        // ---------------------------------------------------------

        cv::imshow("Original", frame);

        // Normaliza o Laplaciano para visualização
        cv::Mat laplacianDisplay;
        cv::normalize(
            laplacianAbs,
            laplacianDisplay,
            0,
            255,
            cv::NORM_MINMAX,
            CV_8U
        );

        cv::imshow("Laplaciano", laplacianDisplay);
        cv::imshow("Resultado", output);

        char key = (char)cv::waitKey(10);

        // ESC encerra
        if (key == 27)
            break;

        // 'r' reinicia os máximos
        if (key == 'r') {

            maxLaplacian = cv::Mat::zeros(
                frameGray.size(),
                CV_32F
            );

            output = frame.clone();

            std::cout << "Maximos reiniciados." << std::endl;
        }
    }

    cap.release();
    cv::destroyAllWindows();

    return 0;
}