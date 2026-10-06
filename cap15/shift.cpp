#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

// Imagem original e imagem resultante
Mat original;
Mat resultado;

// Parâmetros dos trackbars
int foco_altura = 30;       // altura da região em foco (%)
int desfoque = 50;          // força do desfoque (%)
int foco_posicao = 50;      // posição vertical do centro (%)

int foco_altura_max = 100;
int desfoque_max = 100;
int foco_posicao_max = 100;

// Função que aplica o efeito tilt-shift
void aplicaTiltShift()
{
    if (original.empty())
        return;

    int largura = original.cols;
    int altura = original.rows;

    // ---------------------------------------------------------
    // 1. Cria uma versão borrada da imagem
    // ---------------------------------------------------------

    // O tamanho do kernel depende do controle de desfoque
    int ksize = 1 + 2 * (desfoque * 15 / desfoque_max);

    if (ksize < 1)
        ksize = 1;

    if (ksize % 2 == 0)
        ksize++;

    Mat borrada;

    GaussianBlur(
        original,
        borrada,
        Size(ksize, ksize),
        0
    );

    // ---------------------------------------------------------
    // 2. Define a posição e altura da região em foco
    // ---------------------------------------------------------

    double centro =
        (double)foco_posicao / foco_posicao_max * altura;

    double meia_altura =
        ((double)foco_altura / foco_altura_max * altura) / 2.0;

    // ---------------------------------------------------------
    // 3. Cria máscara com transição suave
    // ---------------------------------------------------------

    Mat mascara(altura, largura, CV_32FC1);

    for (int y = 0; y < altura; y++)
    {
        double distancia = abs(y - centro);

        double valor;

        if (distancia <= meia_altura)
        {
            // Região totalmente em foco
            valor = 1.0;
        }
        else
        {
            // Distância a partir do limite da região em foco
            double distanciaFora = distancia - meia_altura;

            // Quanto maior o desfoque, mais rápida é a transição
            double faixaTransicao =
                altura * (0.05 + 0.45 * (desfoque / 100.0));

            if (faixaTransicao < 1.0)
                faixaTransicao = 1.0;

            valor = 1.0 - distanciaFora / faixaTransicao;

            if (valor < 0.0)
                valor = 0.0;
        }

        for (int x = 0; x < largura; x++)
            mascara.at<float>(y, x) = (float)valor;
    }

    // ---------------------------------------------------------
    // 4. Combina imagem original e imagem borrada
    // ---------------------------------------------------------

    resultado = Mat::zeros(original.size(), original.type());

    for (int y = 0; y < altura; y++)
    {
        for (int x = 0; x < largura; x++)
        {
            float peso = mascara.at<float>(y, x);

            Vec3b pixelOriginal = original.at<Vec3b>(y, x);
            Vec3b pixelBorrado = borrada.at<Vec3b>(y, x);

            Vec3b &pixelResultado = resultado.at<Vec3b>(y, x);

            for (int c = 0; c < 3; c++)
            {
                pixelResultado[c] =
                    saturate_cast<uchar>(
                        peso * pixelOriginal[c] +
                        (1.0f - peso) * pixelBorrado[c]
                    );
            }
        }
    }

    // Mostra o resultado
    imshow("Tilt-Shift", resultado);
}

// Callbacks dos trackbars
void onFocoAltura(int, void*)
{
    aplicaTiltShift();
}

void onDesfoque(int, void*)
{
    aplicaTiltShift();
}

void onFocoPosicao(int, void*)
{
    aplicaTiltShift();
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        cout << "Uso: " << argv[0] << " imagem" << endl;
        return 1;
    }

    // Carrega a imagem
    original = imread(argv[1]);

    if (original.empty())
    {
        cerr << "Erro ao abrir a imagem: "
             << argv[1] << endl;
        return 1;
    }

    // Cria janela
    namedWindow("Tilt-Shift", WINDOW_AUTOSIZE);

    // Cria os três controles
    createTrackbar(
        "Altura do foco",
        "Tilt-Shift",
        &foco_altura,
        foco_altura_max,
        onFocoAltura
    );

    createTrackbar(
        "Forca do desfoque",
        "Tilt-Shift",
        &desfoque,
        desfoque_max,
        onDesfoque
    );

    createTrackbar(
        "Posicao do foco",
        "Tilt-Shift",
        &foco_posicao,
        foco_posicao_max,
        onFocoPosicao
    );

    // Aplica o efeito inicialmente
    aplicaTiltShift();

    cout << endl;
    cout << "Controles:" << endl;
    cout << " - Altura do foco: tamanho da regiao nitida" << endl;
    cout << " - Forca do desfoque: intensidade do borramento" << endl;
    cout << " - Posicao do foco: deslocamento vertical da regiao" << endl;
    cout << endl;
    cout << "Pressione 's' para salvar a imagem." << endl;
    cout << "Pressione ESC para sair." << endl;

    while (true)
    {
        char tecla = waitKey(30);

        // Salvar resultado
        if (tecla == 's' || tecla == 'S')
        {
            imwrite("tiltshift_resultado.png", resultado);

            cout << "Imagem salva em: "
                 << "tiltshift_resultado.png"
                 << endl;
        }

        // Sair
        if (tecla == 27)
            break;
    }

    destroyAllWindows();

    return 0;
}