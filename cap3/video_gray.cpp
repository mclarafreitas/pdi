#include <iostream>
#include <opencv2/opencv.hpp>

int main(int argc, char** argv) {

    if (argc < 2) {
        std::cout << "Uso: ./video_gray <video>\n";
        return -1;
    }

    cv::VideoCapture cap(argv[1]);

    if (!cap.isOpened()) {
        std::cout << "Erro ao abrir o video: " << argv[1] << "\n";
        return -1;
    }

    int width = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    int height = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    double fps = cap.get(cv::CAP_PROP_FPS);

    std::cout << "Largura: " << width << "\n";
    std::cout << "Altura: " << height << "\n";
    std::cout << "FPS: " << fps << "\n";

    cv::Size frameSize(width, height);

    // Codec MJPG
    cv::VideoWriter out(
        "output_gray.avi",
        cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),
        fps,
        frameSize,
        false
    );

    if (!out.isOpened()) {
        std::cout << "Erro ao criar o video de saida!\n";
        return -1;
    }

    cv::Mat frame;
    cv::Mat gray;

    int counter = 0;

    while (cap.read(frame)) {

        // Converte BGR para tons de cinza
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        // Grava o frame em tons de cinza
        out.write(gray);

        // Mostra o resultado
        cv::imshow("Video em tons de cinza", gray);

        counter++;

        // Pressione ESC para interromper
        if (cv::waitKey(30) == 27)
            break;
    }

    std::cout << "Numero de frames: " << counter << "\n";

    cap.release();
    out.release();
    cv::destroyAllWindows();

    return 0;
}