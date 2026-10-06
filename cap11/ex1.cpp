#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char** argv) {

    if (argc < 2) {
        cout << "Uso: " << argv[0] << " <imagem>" << endl;
        return -1;
    }

    // Carrega a imagem em tons de cinza
    Mat image = imread(argv[1], IMREAD_GRAYSCALE);

    if (!image.data) {
        cout << "Imagem nao carregou corretamente" << endl;
        return -1;
    }

    cout << image.cols << "x" << image.rows << endl;

    // Cria uma imagem binaria
    // Pixels brancos (255) representam os objetos
    Mat binary;

    threshold(image, binary, 127, 255, THRESH_BINARY);

    // Matriz para armazenar os rotulos
    // CV_32S permite valores muito maiores que 255
    Mat labels;

    // Rotula os componentes conexos
    int nobjects = connectedComponents(
        binary,
        labels,
        8,
        CV_32S
    );

    // O objeto 0 representa o fundo,
    // por isso o numero de objetos e nobjects - 1
    nobjects--;

    cout << "A figura tem "
         << nobjects
         << " bolhas" << endl;

    // Cria uma imagem para visualizacao
    Mat visualization;

    // Normaliza os rotulos para 0-255
    normalize(
        labels,
        visualization,
        0,
        255,
        NORM_MINMAX
    );

    visualization.convertTo(visualization, CV_8U);

    // Mostra a imagem rotulada
    imshow("Imagem rotulada", visualization);

    // Salva o resultado
    imwrite("labeling.png", visualization);

    waitKey();

    return 0;
}