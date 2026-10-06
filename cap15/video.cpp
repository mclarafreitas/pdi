#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

// Parâmetros do efeito
int foco_altura = 30;
int forca_desfoque = 50;
int foco_posicao = 50;

// Função que aplica o efeito tilt-shift em um quadro
Mat aplicaTiltShift(const Mat &original)
{
    int largura = original.cols;
    int altura = original.rows;

    // ---------------------------------------------------------
    // Cria a imagem borrada
    // ---------------------------------------------------------

    int ksize = 1 + 2 * (forca_desfoque * 15 / 100);

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
    // Define a posição e a altura da região em foco
    // ---------------------------------------------------------

    double centro =
        (double)foco_posicao / 100.0 * altura;

    double meia_altura =
        ((double)foco_altura / 100.0 * altura) / 2.0;

    // ---------------------------------------------------------
    // Cria a máscara vertical
    // ---------------------------------------------------------

    Mat mascara(altura, largura, CV_32FC1);

    double faixa_transicao =
        altura * (0.05 + 0.45 * (forca_desfoque / 100.0));

    if (faixa_transicao < 1.0)
        faixa_transicao = 1.0;

    for (int y = 0; y < altura; y++)
    {
        double distancia = abs(y - centro);

        double peso;

        // Região completamente em foco
        if (distancia <= meia_altura)
        {
            peso = 1.0;
        }
        else
        {
            double distancia_fora =
                distancia - meia_altura;

            peso =
                1.0 - distancia_fora / faixa_transicao;

            if (peso < 0.0)
                peso = 0.0;
        }

        for (int x = 0; x < largura; x++)
        {
            mascara.at<float>(y, x) =
                (float)peso;
        }
    }

    // ---------------------------------------------------------
    // Combina imagem original e imagem borrada
    // ---------------------------------------------------------

    Mat resultado = Mat::zeros(
        original.size(),
        original.type()
    );

    for (int y = 0; y < altura; y++)
    {
        for (int x = 0; x < largura; x++)
        {
            float peso = mascara.at<float>(y, x);

            Vec3b pixel_original =
                original.at<Vec3b>(y, x);

            Vec3b pixel_borrado =
                borrada.at<Vec3b>(y, x);

            Vec3b &pixel_resultado =
                resultado.at<Vec3b>(y, x);

            for (int c = 0; c < 3; c++)
            {
                pixel_resultado[c] =
                    saturate_cast<uchar>(
                        peso * pixel_original[c] +
                        (1.0f - peso) *
                        pixel_borrado[c]
                    );
            }
        }
    }

    return resultado;
}

int main(int argc, char **argv)
{
    // ---------------------------------------------------------
    // Verificação dos argumentos
    // ---------------------------------------------------------

    if (argc < 3)
    {
        cout << "Uso: " << argv[0]
             << " video_entrada video_saida"
             << endl;

        cout << "Exemplo:"
             << endl;

        cout << "./tiltshiftvideo entrada.mp4 saida.mp4"
             << endl;

        return 1;
    }

    // ---------------------------------------------------------
    // Abre o vídeo
    // ---------------------------------------------------------

    VideoCapture video(argv[1]);

    if (!video.isOpened())
    {
        cerr << "Erro ao abrir o video: "
             << argv[1] << endl;

        return 1;
    }

    // Informações do vídeo original
    int largura =
        (int)video.get(CAP_PROP_FRAME_WIDTH);

    int altura =
        (int)video.get(CAP_PROP_FRAME_HEIGHT);

    double fps =
        video.get(CAP_PROP_FPS);

    int total_quadros =
        (int)video.get(CAP_PROP_FRAME_COUNT);

    cout << "Video original:" << endl;
    cout << "Resolucao: "
         << largura << "x" << altura << endl;

    cout << "FPS: "
         << fps << endl;

    cout << "Quadros: "
         << total_quadros << endl;

    // ---------------------------------------------------------
    // Stop motion
    //
    // Processamos somente um quadro a cada N quadros.
    // Quanto maior o valor, mais evidente o efeito.
    // ---------------------------------------------------------

    int salto = 4;

    // Reduzimos o FPS proporcionalmente ao descarte
    double fps_saida = fps / salto;

    // ---------------------------------------------------------
    // Cria o arquivo de saída
    // ---------------------------------------------------------

    int codec = VideoWriter::fourcc(
        'm', 'p', '4', 'v'
    );

    VideoWriter saida(
        argv[2],
        codec,
        fps_saida,
        Size(largura, altura)
    );

    if (!saida.isOpened())
    {
        cerr << "Erro ao criar o video de saida."
             << endl;

        return 1;
    }

    // ---------------------------------------------------------
    // Janela para acompanhar o processamento
    // ---------------------------------------------------------

    namedWindow(
        "Tilt-Shift Video",
        WINDOW_AUTOSIZE
    );

    Mat frame;
    Mat resultado;

    int numero_quadro = 0;
    int quadros_processados = 0;

    // ---------------------------------------------------------
    // Processamento
    // ---------------------------------------------------------

    while (true)
    {
        if (!video.read(frame))
            break;

        // -----------------------------------------------------
        // Descarta quadros para criar stop motion
        // -----------------------------------------------------

        if (numero_quadro % salto != 0)
        {
            numero_quadro++;
            continue;
        }

        // Aplica tilt-shift
        resultado = aplicaTiltShift(frame);

        // Escreve o quadro processado
        saida.write(resultado);

        // Mostra na tela
        imshow(
            "Tilt-Shift Video",
            resultado
        );

        quadros_processados++;

        numero_quadro++;

        // ESC encerra o processamento
        char tecla = (char)waitKey(1);

        if (tecla == 27)
            break;
    }

    // ---------------------------------------------------------
    // Finalização
    // ---------------------------------------------------------

    video.release();
    saida.release();

    destroyAllWindows();

    cout << endl;
    cout << "Processamento concluido!" << endl;

    cout << "Quadros originais: "
         << numero_quadro << endl;

    cout << "Quadros processados: "
         << quadros_processados << endl;

    cout << "Video salvo em: "
         << argv[2] << endl;

    return 0;
}