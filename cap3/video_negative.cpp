#include <iostream>
#include <opencv2/opencv.hpp>

int main(int argc, char** argv) {

    if (argc < 2) {
        std::cout << "Uso: ./video_negative <video>\n";
        return -1;
    }

    cv::VideoCapture cap(argv[1]);

    if (!cap.isOpened()) {
        std::cout << "Erro ao abrir o video!\n";
        return -1;
    }

    int width = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    int height = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    double fps = cap.get(cv::CAP_PROP_FPS);

    std::cout << "Largura = " << width << "\n";
    std::cout << "Altura = " << height << "\n";
    std::cout << "FPS = " << fps << "\n";

    cv::Size frameSize(width, height);

    cv::VideoWriter out(
        "output_negative.avi",
        cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),
        fps,
        frameSize,
        true
    );

    if (!out.isOpened()) {
        std::cout << "Erro ao criar o video de saida!\n";
        return -1;
    }

    cv::Mat frame;
    cv::Mat negative;

    int counter = 0;

    while (cap.read(frame)) {

        // Aplica o negativo ao frame
        cv::bitwise_not(frame, negative);

        // Grava o frame negativo
        out.write(negative);

        // Mostra o resultado
        cv::imshow("Video negativo", negative);

        counter++;

        // Pressione ESC para encerrar
        if (cv::waitKey(30) == 27)
            break;
    }

    std::cout << "Numero de frames: " << counter << "\n";

    cap.release();
    out.release();
    cv::destroyAllWindows();

    return 0;
}