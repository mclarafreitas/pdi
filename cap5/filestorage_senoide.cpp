#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <cmath>

using namespace std;

const int SIDE = 256;
const int PERIODOS = 4;
const double PI = 3.14159265358979323846;

int main() {

    string ymlFile = "senoide-256.yml";
    string pngFile = "senoide-256.png";

    // -------------------------------------------------
    // 1. Criar a imagem em ponto flutuante
    // -------------------------------------------------

    cv::Mat image = cv::Mat::zeros(SIDE, SIDE, CV_32FC1);

    for (int i = 0; i < SIDE; i++) {
        for (int j = 0; j < SIDE; j++) {

            image.at<float>(i, j) =
                127.0f * sin(2.0 * PI * PERIODOS * j / SIDE)
                + 128.0f;
        }
    }

    // -------------------------------------------------
    // 2. Salvar a imagem original no YML
    // -------------------------------------------------

    cv::FileStorage fs(ymlFile, cv::FileStorage::WRITE);

    if (!fs.isOpened()) {
        cerr << "Erro ao abrir o arquivo YML." << endl;
        return -1;
    }

    fs << "mat" << image;
    fs.release();

    // -------------------------------------------------
    // 3. Preparar e salvar a imagem como PNG
    // -------------------------------------------------

    cv::Mat imagePNG;

    cv::normalize(
        image,
        imagePNG,
        0,
        255,
        cv::NORM_MINMAX
    );

    imagePNG.convertTo(imagePNG, CV_8U);

    if (!cv::imwrite(pngFile, imagePNG)) {
        cerr << "Erro ao salvar PNG." << endl;
        return -1;
    }

    // -------------------------------------------------
    // 4. Ler novamente o YML
    // -------------------------------------------------

    cv::Mat imageYML;

    fs.open(ymlFile, cv::FileStorage::READ);

    if (!fs.isOpened()) {
        cerr << "Erro ao abrir YML para leitura." << endl;
        return -1;
    }

    fs["mat"] >> imageYML;
    fs.release();

    // -------------------------------------------------
    // 5. Escolher uma linha para comparação
    // -------------------------------------------------

    int linha = 128;

    ofstream arquivoDiff("diferenca.txt");

    if (!arquivoDiff.is_open()) {
        cerr << "Erro ao criar diferenca.txt." << endl;
        return -1;
    }

    // -------------------------------------------------
    // 6. Comparar YML x PNG
    // -------------------------------------------------

    double maxDiff = 0.0;
    double somaDiff = 0.0;

    for (int j = 0; j < SIDE; j++) {

        float valorYML = imageYML.at<float>(linha, j);
        float valorPNG = imagePNG.at<uchar>(linha, j);

        float diferenca = valorYML - valorPNG;

        arquivoDiff << j << " " << diferenca << endl;

        maxDiff = max(maxDiff, abs((double)diferenca));
        somaDiff += abs((double)diferenca);
    }

    arquivoDiff.close();

    cout << "Imagem gerada: " << pngFile << endl;
    cout << "Arquivo YML: " << ymlFile << endl;
    cout << "Linha comparada: " << linha << endl;
    cout << "Maior diferenca absoluta: " << maxDiff << endl;
    cout << "Diferenca absoluta media: "
         << somaDiff / SIDE << endl;

    // -------------------------------------------------
    // 7. Mostrar as imagens
    // -------------------------------------------------

    cv::imshow("Senoide", imagePNG);
    cv::waitKey(0);

    return 0;
}