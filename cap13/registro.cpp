#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

std::vector<cv::Point2f> pontos;

// Callback do mouse
void mouseCallback(int event, int x, int y, int flags, void* userdata)
{
    if (event == cv::EVENT_LBUTTONDOWN && pontos.size() < 4)
    {
        pontos.push_back(cv::Point2f(x, y));

        std::cout << "Ponto " << pontos.size()
                  << ": (" << x << ", " << y << ")" << std::endl;

        cv::Mat* imagem = static_cast<cv::Mat*>(userdata);

        // Desenha o ponto
        cv::circle(
            *imagem,
            cv::Point(x, y),
            5,
            cv::Scalar(0, 0, 255),
            -1
        );

        // Número do ponto
        cv::putText(
            *imagem,
            std::to_string(pontos.size()),
            cv::Point(x + 10, y),
            cv::FONT_HERSHEY_SIMPLEX,
            0.7,
            cv::Scalar(0, 255, 0),
            2
        );

        cv::imshow("Selecione os pontos", *imagem);
    }
}

int main()
{
    // Carrega a imagem
    cv::Mat image = cv::imread("voltimetro.png");

    if (image.empty())
    {
        std::cerr << "Erro ao carregar orca.jpg" << std::endl;
        return -1;
    }

    cv::Mat imagemSelecao = image.clone();

    std::cout << "Selecione os quatro pontos na imagem.\n";
    std::cout << "Ordem:\n";
    std::cout << "1 - Superior esquerdo\n";
    std::cout << "2 - Superior direito\n";
    std::cout << "3 - Inferior direito\n";
    std::cout << "4 - Inferior esquerdo\n\n";

    cv::namedWindow("Selecione os pontos");

    cv::setMouseCallback(
        "Selecione os pontos",
        mouseCallback,
        &imagemSelecao
    );

    cv::imshow("Selecione os pontos", imagemSelecao);

    // Aguarda os quatro cliques
    while (pontos.size() < 4)
    {
        int tecla = cv::waitKey(30);

        if (tecla == 27)
        {
            std::cout << "Operacao cancelada." << std::endl;
            return 0;
        }
    }

    std::cout << "\nQuatro pontos selecionados!\n";

    // Calcula largura
    int largura = std::max(
        (int)cv::norm(pontos[0] - pontos[1]),
        (int)cv::norm(pontos[2] - pontos[3])
    );

    // Calcula altura
    int altura = std::max(
        (int)cv::norm(pontos[1] - pontos[2]),
        (int)cv::norm(pontos[3] - pontos[0])
    );

    std::cout << "Largura: " << largura << std::endl;
    std::cout << "Altura: " << altura << std::endl;

    /*
     * Pontos de destino
     *
     * 0 ---------------- 1
     * |                  |
     * |                  |
     * 3 ---------------- 2
     */
    std::vector<cv::Point2f> destino;

    destino.push_back(cv::Point2f(0, 0));
    destino.push_back(cv::Point2f(largura - 1, 0));
    destino.push_back(cv::Point2f(largura - 1, altura - 1));
    destino.push_back(cv::Point2f(0, altura - 1));

    /*
     * Monta o sistema para calcular a homografia.
     *
     * Para cada ponto:
     *
     * x' = (h11*x + h12*y + h13) /
     *      (h31*x + h32*y + 1)
     *
     * y' = (h21*x + h22*y + h23) /
     *      (h31*x + h32*y + 1)
     */

    cv::Mat A(8, 8, CV_64F);
    cv::Mat B(8, 1, CV_64F);

    for (int i = 0; i < 4; i++)
    {
        double x = pontos[i].x;
        double y = pontos[i].y;

        double xp = destino[i].x;
        double yp = destino[i].y;

        int linha = 2 * i;

        // Equação para x'
        A.at<double>(linha, 0) = x;
        A.at<double>(linha, 1) = y;
        A.at<double>(linha, 2) = 1;
        A.at<double>(linha, 3) = 0;
        A.at<double>(linha, 4) = 0;
        A.at<double>(linha, 5) = 0;
        A.at<double>(linha, 6) = -x * xp;
        A.at<double>(linha, 7) = -y * xp;

        B.at<double>(linha, 0) = xp;

        // Equação para y'
        A.at<double>(linha + 1, 0) = 0;
        A.at<double>(linha + 1, 1) = 0;
        A.at<double>(linha + 1, 2) = 0;
        A.at<double>(linha + 1, 3) = x;
        A.at<double>(linha + 1, 4) = y;
        A.at<double>(linha + 1, 5) = 1;
        A.at<double>(linha + 1, 6) = -x * yp;
        A.at<double>(linha + 1, 7) = -y * yp;

        B.at<double>(linha + 1, 0) = yp;
    }

    // Resolve o sistema
    cv::Mat h;

    cv::solve(
        A,
        B,
        h,
        cv::DECOMP_LU
    );

    // Monta a matriz de perspectiva
    cv::Mat perspectiveMatrix = cv::Mat::eye(3, 3, CV_64F);

    perspectiveMatrix.at<double>(0, 0) = h.at<double>(0);
    perspectiveMatrix.at<double>(0, 1) = h.at<double>(1);
    perspectiveMatrix.at<double>(0, 2) = h.at<double>(2);

    perspectiveMatrix.at<double>(1, 0) = h.at<double>(3);
    perspectiveMatrix.at<double>(1, 1) = h.at<double>(4);
    perspectiveMatrix.at<double>(1, 2) = h.at<double>(5);

    perspectiveMatrix.at<double>(2, 0) = h.at<double>(6);
    perspectiveMatrix.at<double>(2, 1) = h.at<double>(7);

    std::cout << "\nMatriz de perspectiva:\n";
    std::cout << perspectiveMatrix << std::endl;

    // Corrige a perspectiva
    cv::Mat correctedImage;

    cv::warpPerspective(
        image,
        correctedImage,
        perspectiveMatrix,
        cv::Size(largura, altura)
    );

    // Mostra resultado
    cv::imshow("Original Image", image);
    cv::imshow("Corrected Image", correctedImage);

    cv::waitKey(0);

    return 0;
}