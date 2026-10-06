#include <iostream>
#include <opencv2/opencv.hpp>

cv::Mat image;
cv::Mat selectedMask;

bool painting = false;
bool removing = false;

cv::Point previousPoint;

cv::Scalar average;
cv::Scalar stdev;

int rangeR = 0;
int rangeG = 0;
int rangeB = 0;

void on_trackbar_R(int value, void*) {
    rangeR = value;
}

void on_trackbar_G(int value, void*) {
    rangeG = value;
}

void on_trackbar_B(int value, void*) {
    rangeB = value;
}

// Desenha a seleção sobre a imagem
void showSelection() {
    cv::Mat display;
    image.copyTo(display);

    // Coloca a seleção em vermelho
    cv::Mat red(display.size(), display.type(), cv::Scalar(0, 0, 255));
    red.copyTo(display, selectedMask);

    cv::addWeighted(image, 0.5, display, 0.5, 0, display);

    cv::imshow("Image", display);
}

// Callback do mouse
static void onMouse(int event, int x, int y, int flags, void*) {

    if (event == cv::EVENT_LBUTTONDOWN) {
        painting = true;
        removing = false;
        previousPoint = cv::Point(x, y);

        // Adiciona o ponto à seleção
        cv::circle(selectedMask, cv::Point(x, y), 5,
                   cv::Scalar(255), -1);
    }

    else if (event == cv::EVENT_RBUTTONDOWN) {
        removing = true;
        painting = true;
        previousPoint = cv::Point(x, y);

        // Remove o ponto da seleção
        cv::circle(selectedMask, cv::Point(x, y), 5,
                   cv::Scalar(0), -1);
    }

    else if (event == cv::EVENT_MOUSEMOVE && painting) {

        cv::Point currentPoint(x, y);

        if (removing) {
            // Apaga a seleção
            cv::line(selectedMask, previousPoint, currentPoint,
                     cv::Scalar(0), 10);
        } else {
            // Adiciona à seleção
            cv::line(selectedMask, previousPoint, currentPoint,
                     cv::Scalar(255), 10);
        }

        previousPoint = currentPoint;
    }

    else if (event == cv::EVENT_LBUTTONUP ||
             event == cv::EVENT_RBUTTONUP) {

        painting = false;
        removing = false;
    }
}

// Calcula média e desvio apenas dos pixels selecionados
void calculateStatistics() {

    cv::meanStdDev(image, average, stdev, selectedMask);

    rangeR = cv::saturate_cast<int>(3 * stdev[2]);
    rangeG = cv::saturate_cast<int>(3 * stdev[1]);
    rangeB = cv::saturate_cast<int>(3 * stdev[0]);

    cv::setTrackbarPos("R-range", "Image", rangeR);
    cv::setTrackbarPos("G-range", "Image", rangeG);
    cv::setTrackbarPos("B-range", "Image", rangeB);

    std::cout << "\nAverage: " << average << std::endl;

    std::cout << "Range min: "
              << average - cv::Scalar(rangeB, rangeG, rangeR)
              << std::endl;

    std::cout << "Range max: "
              << average + cv::Scalar(rangeB, rangeG, rangeR)
              << std::endl;
}

int main(int argc, const char** argv) {

    if (argc < 2) {
        std::cout << "Uso: " << argv[0] << " imagem" << std::endl;
        return -1;
    }

    image = cv::imread(argv[1]);

    if (image.empty()) {
        std::cout << "Erro ao carregar a imagem." << std::endl;
        return -1;
    }

    // Máscara inicialmente vazia
    selectedMask = cv::Mat::zeros(image.size(), CV_8UC1);

    average = cv::Scalar(128, 128, 128);
    stdev = cv::Scalar(0, 0, 0);

    cv::namedWindow("Image", cv::WINDOW_NORMAL);
    cv::namedWindow("Selection", cv::WINDOW_NORMAL);

    cv::createTrackbar(
        "R-range", "Image",
        &rangeR, 255,
        on_trackbar_R
    );

    cv::createTrackbar(
        "G-range", "Image",
        &rangeG, 255,
        on_trackbar_G
    );

    cv::createTrackbar(
        "B-range", "Image",
        &rangeB, 255,
        on_trackbar_B
    );

    cv::setMouseCallback("Image", onMouse);

    cv::Mat mask;

    while (true) {

        // Mostra a imagem com a seleção
        showSelection();

        // Gera a máscara de seleção de cores
        cv::inRange(
            image,
            average - cv::Scalar(rangeB, rangeG, rangeR),
            average + cv::Scalar(rangeB, rangeG, rangeR),
            mask
        );

        cv::imshow("Selection", mask);

        char c = cv::waitKey(10);

        // ESC encerra
        if (c == 27) {
            break;
        }

        // ENTER ou ESPAÇO calcula novamente as estatísticas
        if (c == 13 || c == ' ') {

            if (cv::countNonZero(selectedMask) > 0) {

                calculateStatistics();

                std::cout
                    << "Pixels selecionados: "
                    << cv::countNonZero(selectedMask)
                    << std::endl;
            }
            else {
                std::cout
                    << "Nenhum pixel foi selecionado."
                    << std::endl;
            }
        }

        // C limpa toda a seleção
        if (c == 'c' || c == 'C') {
            selectedMask.setTo(0);
            std::cout << "Selecao limpa." << std::endl;
        }
    }

    cv::imwrite("selection.png", mask);
    cv::imwrite("selected_pixels.png", selectedMask);

    return 0;
}