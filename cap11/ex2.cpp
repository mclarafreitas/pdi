#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char** argv) {

    if (argc < 2) {
        cout << "Uso: " << argv[0] << " <imagem>" << endl;
        return -1;
    }

    // Carrega a imagem
    Mat image = imread(argv[1], IMREAD_GRAYSCALE);

    if (image.empty()) {
        cout << "Imagem nao carregou corretamente" << endl;
        return -1;
    }

    cout << image.cols << "x" << image.rows << endl;

    // ============================================================
    // 1. Binarizacao
    // ============================================================

    Mat binary;

    threshold(image, binary, 127, 255, THRESH_BINARY);

    // ============================================================
    // 2. Rotulacao dos objetos
    // ============================================================

    Mat labels;

    int nobjects = connectedComponents(
        binary,
        labels,
        8,
        CV_32S
    );

    nobjects--;

    cout << "Objetos encontrados: "
         << nobjects << endl;

    // Contadores
    int validObjects = 0;
    int objectsWithHoles = 0;
    int objectsWithoutHoles = 0;
    int objectsWithMultipleHoles = 0;

    // ============================================================
    // 3. Analisa cada objeto
    // ============================================================

    for (int obj = 1; obj <= nobjects; obj++) {

        // --------------------------------------------------------
        // Verifica se o objeto toca a borda da imagem
        // --------------------------------------------------------

        bool touchesBorder = false;

        // Borda superior e inferior
        for (int x = 0; x < image.cols; x++) {

            if (labels.at<int>(0, x) == obj ||
                labels.at<int>(image.rows - 1, x) == obj) {

                touchesBorder = true;
                break;
            }
        }

        // Borda esquerda e direita
        if (!touchesBorder) {

            for (int y = 0; y < image.rows; y++) {

                if (labels.at<int>(y, 0) == obj ||
                    labels.at<int>(y, image.cols - 1) == obj) {

                    touchesBorder = true;
                    break;
                }
            }
        }

        // Se toca a borda, ignora
        if (touchesBorder) {
            continue;
        }

        validObjects++;

        // --------------------------------------------------------
        // Cria máscara do objeto
        // --------------------------------------------------------

        Mat objectMask = (labels == obj);

        // --------------------------------------------------------
        // Encontra os limites do objeto manualmente
        // --------------------------------------------------------

        int minX = image.cols;
        int maxX = 0;
        int minY = image.rows;
        int maxY = 0;

        for (int y = 0; y < image.rows; y++) {

            for (int x = 0; x < image.cols; x++) {

                if (labels.at<int>(y, x) == obj) {

                    if (x < minX) minX = x;
                    if (x > maxX) maxX = x;
                    if (y < minY) minY = y;
                    if (y > maxY) maxY = y;
                }
            }
        }

        // --------------------------------------------------------
        // Cria uma região somente com o objeto
        // --------------------------------------------------------

        Rect roiRect(
            minX,
            minY,
            maxX - minX + 1,
            maxY - minY + 1
        );

        Mat roi = objectMask(roiRect);

        // --------------------------------------------------------
        // Inverte a imagem
        //
        // Objeto = preto
        // Fundo/buracos = branco
        // --------------------------------------------------------

        Mat inverted;

        bitwise_not(roi, inverted);

        // --------------------------------------------------------
        // Rotula as regiões brancas
        // --------------------------------------------------------

        Mat holeLabels;

        int nregions = connectedComponents(
            inverted,
            holeLabels,
            8,
            CV_32S
        );

        int holes = 0;

        // --------------------------------------------------------
        // Procura regiões que NÃO tocam a borda da ROI
        //
        // Uma região branca que toca a borda é o fundo externo.
        // Uma região branca fechada é um buraco.
        // --------------------------------------------------------

        for (int region = 1; region < nregions; region++) {

            bool touchesRoiBorder = false;

            // Borda superior/inferior da ROI
            for (int x = 0; x < holeLabels.cols; x++) {

                if (holeLabels.at<int>(0, x) == region ||
                    holeLabels.at<int>(holeLabels.rows - 1, x) == region) {

                    touchesRoiBorder = true;
                    break;
                }
            }

            // Borda esquerda/direita da ROI
            if (!touchesRoiBorder) {

                for (int y = 0; y < holeLabels.rows; y++) {

                    if (holeLabels.at<int>(y, 0) == region ||
                        holeLabels.at<int>(y, holeLabels.cols - 1) == region) {

                        touchesRoiBorder = true;
                        break;
                    }
                }
            }

            // Se não toca a borda, é um buraco
            if (!touchesRoiBorder) {
                holes++;
            }
        }

        // --------------------------------------------------------
        // Classificação do objeto
        // --------------------------------------------------------

        if (holes == 0) {

            objectsWithoutHoles++;

            cout << "Objeto " << obj
                 << ": sem buracos" << endl;

        } else {

            objectsWithHoles++;

            cout << "Objeto " << obj
                 << ": " << holes
                 << " buraco(s)" << endl;

            if (holes > 1) {
                objectsWithMultipleHoles++;
            }
        }
    }

    // ============================================================
    // 4. Resultado
    // ============================================================

    cout << endl;
    cout << "========== RESULTADO ==========" << endl;

    cout << "Objetos validos: "
         << validObjects << endl;

    cout << "Objetos sem buracos: "
         << objectsWithoutHoles << endl;

    cout << "Objetos com buracos: "
         << objectsWithHoles << endl;

    cout << "Objetos com mais de um buraco: "
         << objectsWithMultipleHoles << endl;

    // ============================================================
    // 5. Visualizacao
    // ============================================================

    Mat visualization;

    normalize(
        labels,
        visualization,
        0,
        255,
        NORM_MINMAX
    );

    visualization.convertTo(visualization, CV_8U);

    imshow("Objetos rotulados", visualization);

    imwrite("labeling_holes.png", visualization);

    waitKey();

    return 0;
}