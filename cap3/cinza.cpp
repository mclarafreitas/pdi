#include <iostream>
#include <opencv2/opencv.hpp>

int main(int argc, char** argv) {

    cv::VideoCapture cap;
    double width, height;
    cv::Mat frame, gray;
    int counter;

    cap.open(argv[1]);

    if (!cap.isOpened())
        return -1;

    width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);

    std::cout << "largura = " << width << "\n";
    std::cout << "altura = " << height << "\n";

    cv::Size frameSize(static_cast<int>(width), static_cast<int>(height));

    // Codec
    int type = cap.get(cv::CAP_PROP_FOURCC);

    // Vídeo de saída em tons de cinza
    cv::VideoWriter out(
        "output_gray.avi",
        type,
        cap.get(cv::CAP_PROP_FPS),
        frameSize,
        false
    );

    if (!out.isOpened()) {
        std::cout << "Erro ao abrir o arquivo de saída!\n";
        return -1;
    }

    for (counter = 0; cap.read(frame); counter++) {

        // Converte o frame colorido para tons de cinza
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        // Grava o frame em tons de cinza
        out << gray;

        // Mostra o resultado
        cv::imshow("Video em tons de cinza", gray);

        if (cv::waitKey(30) >= 0)
            break;
    }

    std::cout << "Numero de frames: " << counter << "\n";

    cap.release();
    out.release();
    cv::destroyAllWindows();

    return 0;
}