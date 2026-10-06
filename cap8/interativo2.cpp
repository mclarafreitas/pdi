#include <iostream>
#include <opencv2/opencv.hpp>

cv::Mat image;
cv::Mat selectedMask;
cv::Mat colorMask;

bool painting = false;
bool removing = false;

cv::Point previousPoint;


// ---------------------------------------------------------
// Mouse
// ---------------------------------------------------------

static void onMouse(int event, int x, int y, int flags, void*)
{
    // Botão esquerdo: adicionar
    if (event == cv::EVENT_LBUTTONDOWN)
    {
        painting = true;
        removing = false;

        previousPoint = cv::Point(x, y);

        cv::circle(
            selectedMask,
            cv::Point(x, y),
            8,
            cv::Scalar(255),
            -1
        );
    }

    // Botão direito: remover
    else if (event == cv::EVENT_RBUTTONDOWN)
    {
        painting = true;
        removing = true;

        previousPoint = cv::Point(x, y);

        cv::circle(
            selectedMask,
            cv::Point(x, y),
            8,
            cv::Scalar(0),
            -1
        );
    }

    // Arrastando o mouse
    else if (event == cv::EVENT_MOUSEMOVE && painting)
    {
        cv::Point currentPoint(x, y);

        if (removing)
        {
            // Remove pixels
            cv::line(
                selectedMask,
                previousPoint,
                currentPoint,
                cv::Scalar(0),
                15
            );
        }
        else
        {
            // Adiciona pixels
            cv::line(
                selectedMask,
                previousPoint,
                currentPoint,
                cv::Scalar(255),
                15
            );
        }

        previousPoint = currentPoint;
    }

    // Soltou o botão
    else if (
        event == cv::EVENT_LBUTTONUP ||
        event == cv::EVENT_RBUTTONUP
    )
    {
        painting = false;
        removing = false;
    }
}


// ---------------------------------------------------------
// Calcula a faixa de cores
// ---------------------------------------------------------

void calculateColorRange()
{
    if (cv::countNonZero(selectedMask) == 0)
    {
        std::cout << "Nenhum pixel selecionado." << std::endl;
        return;
    }

    int minB = 255;
    int minG = 255;
    int minR = 255;

    int maxB = 0;
    int maxG = 0;
    int maxR = 0;

    // Percorre a imagem
    for (int y = 0; y < image.rows; y++)
    {
        for (int x = 0; x < image.cols; x++)
        {
            // Só utiliza os pixels pintados
            if (selectedMask.at<uchar>(y, x) > 0)
            {
                cv::Vec3b pixel = image.at<cv::Vec3b>(y, x);

                int B = pixel[0];
                int G = pixel[1];
                int R = pixel[2];

                if (B < minB) minB = B;
                if (G < minG) minG = G;
                if (R < minR) minR = R;

                if (B > maxB) maxB = B;
                if (G > maxG) maxG = G;
                if (R > maxR) maxR = R;
            }
        }
    }

    // Pequena tolerância
    int tolerance = 10;

    cv::Scalar lower(
        std::max(0, minB - tolerance),
        std::max(0, minG - tolerance),
        std::max(0, minR - tolerance)
    );

    cv::Scalar upper(
        std::min(255, maxB + tolerance),
        std::min(255, maxG + tolerance),
        std::min(255, maxR + tolerance)
    );

    std::cout << std::endl;

    std::cout << "Pixels selecionados: "
              << cv::countNonZero(selectedMask)
              << std::endl;

    std::cout << "BGR minimo: "
              << minB << ", "
              << minG << ", "
              << minR
              << std::endl;

    std::cout << "BGR maximo: "
              << maxB << ", "
              << maxG << ", "
              << maxR
              << std::endl;

    std::cout << "Limite inferior: "
              << lower
              << std::endl;

    std::cout << "Limite superior: "
              << upper
              << std::endl;

    // Aplica a faixa de cores
    cv::inRange(
        image,
        lower,
        upper,
        colorMask
    );
}


// ---------------------------------------------------------
// Mostra a seleção em vermelho
// ---------------------------------------------------------

void showImage()
{
    cv::Mat display;

    image.copyTo(display);

    cv::Mat red(
        image.size(),
        image.type(),
        cv::Scalar(0, 0, 255)
    );

    cv::Mat overlay;

    cv::addWeighted(
        image,
        0.5,
        red,
        0.5,
        0,
        overlay
    );

    overlay.copyTo(
        display,
        selectedMask
    );

    cv::imshow(
        "Image",
        display
    );
}


// ---------------------------------------------------------
// Main
// ---------------------------------------------------------

int main(int argc, const char** argv)
{
    if (argc < 2)
    {
        std::cout
            << "Uso: "
            << argv[0]
            << " imagem"
            << std::endl;

        return -1;
    }

    image = cv::imread(argv[1]);

    if (image.empty())
    {
        std::cout
            << "Erro ao carregar a imagem."
            << std::endl;

        return -1;
    }

    // Máscara da seleção manual
    selectedMask = cv::Mat::zeros(
        image.size(),
        CV_8UC1
    );

    // Máscara final
    colorMask = cv::Mat::zeros(
        image.size(),
        CV_8UC1
    );

    cv::namedWindow(
        "Image",
        cv::WINDOW_NORMAL
    );

    cv::namedWindow(
        "Selection",
        cv::WINDOW_NORMAL
    );

    cv::setMouseCallback(
        "Image",
        onMouse
    );

    while (true)
    {
        // Mostra imagem + seleção
        showImage();

        // Mostra resultado
        cv::imshow(
            "Selection",
            colorMask
        );

        char key = cv::waitKey(10);

        // ESC
        if (key == 27)
        {
            break;
        }

        // ENTER
        if (key == 13)
        {
            calculateColorRange();
        }

        // ESPAÇO
        if (key == ' ')
        {
            calculateColorRange();
        }

        // C = limpar
        if (key == 'c' || key == 'C')
        {
            selectedMask.setTo(0);
            colorMask.setTo(0);

            std::cout
                << "Selecao limpa."
                << std::endl;
        }
    }

    cv::imwrite(
        "selection.png",
        colorMask
    );

    cv::imwrite(
        "selected_pixels.png",
        selectedMask
    );

    cv::destroyAllWindows();

    return 0;
}