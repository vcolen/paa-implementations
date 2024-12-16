#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <unordered_map>
#include <random>
#include <chrono>

using namespace cv;
using namespace std;
using namespace chrono;

/**
 * Calcula o peso (similaridade) entre dois pixels com base na diferença de cores RGB.
 * @param c1 Vetor de cores do pixel 1 (BGR).
 * @param c2 Vetor de cores do pixel 2 (BGR).
 * @return Peso calculado como a distância Euclidiana no espaço de cores RGB.
 */
float computeWeight(const Vec3b& c1, const Vec3b& c2) {
    float diffB = (float)c1[0] - (float)c2[0];
    float diffG = (float)c1[1] - (float)c2[1];
    float diffR = (float)c1[2] - (float)c2[2];
    return sqrt(diffB * diffB + diffG * diffG + diffR * diffR);
}

/**
 * Classe para gerenciamento de conjuntos disjuntos (Union-Find) usada para fusão de segmentos.
 */
struct DisjointSets {
    vector<int> parent, rank, size;

    /**
     * Construtor que inicializa N conjuntos independentes.
     * @param n Número de elementos.
     */
    DisjointSets(int n) : parent(n), rank(n, 0), size(n, 1) {
        for (int i = 0; i < n; ++i) parent[i] = i;
    }

    /**
     * Encontra o representante do conjunto ao qual um elemento pertence.
     * @param u Elemento para busca.
     * @return Representante do conjunto.
     */
    int findSet(int u) {
        if (parent[u] != u) parent[u] = findSet(parent[u]); // Compressão de caminho.
        return parent[u];
    }

    /**
     * Une dois conjuntos representados por u e v.
     * @param u Representante do conjunto 1.
     * @param v Representante do conjunto 2.
     */
    void unite(int u, int v) {
        u = findSet(u), v = findSet(v);
        if (u != v) {
            if (rank[u] > rank[v]) parent[v] = u, size[u] += size[v];
            else parent[u] = v, size[v] += size[u], rank[u] == rank[v] ? rank[v]++ : 0;
        }
    }

    /**
     * Obtém o tamanho do conjunto ao qual um elemento pertence.
     * @param u Elemento para busca.
     * @return Tamanho do conjunto.
     */
    int getSize(int u) { return size[findSet(u)]; }
};

/**
 * Implementa o algoritmo de segmentação Felzenszwalb-Huttenlocher (FH).
 * @param image Imagem de entrada (em formato BGR).
 * @param k Parâmetro de controle para fusão de segmentos.
 * @param edgeCount Número de arestas do grafo construído.
 * @return Vetor contendo os rótulos de cada pixel após a segmentação.
 */
vector<int> segmentImageFH(const Mat& image, float k, int& edgeCount) {
    int V = image.rows * image.cols; // Número total de pixels.
    vector<int> labels(V); // Vetor de rótulos.
    for (int i = 0; i < V; ++i) labels[i] = i;

    DisjointSets ds(V); // Estrutura Union-Find.
    vector<pair<float, pair<int, int>>> edges; // Lista de arestas no formato (peso, (nó1, nó2)).

    // Construção do grafo: conectando pixels adjacentes com pesos baseados na diferença de cor.
    for (int i = 0; i < image.rows; ++i) {
        for (int j = 0; j < image.cols; ++j) {
            Vec3b c1 = image.at<Vec3b>(i, j);
            if (i + 1 < image.rows) 
                edges.push_back({computeWeight(c1, image.at<Vec3b>(i + 1, j)), {i * image.cols + j, (i + 1) * image.cols + j}});
            if (j + 1 < image.cols) 
                edges.push_back({computeWeight(c1, image.at<Vec3b>(i, j + 1)), {i * image.cols + j, i * image.cols + (j + 1)}});
        }
    }

    edgeCount = edges.size(); // Número total de arestas no grafo.
    sort(edges.begin(), edges.end()); // Ordena as arestas pelo peso.

    vector<float> threshold(V, k); // Vetor de limiares adaptativos.
    for (auto& edge : edges) {
        float w = edge.first;
        int u = edge.second.first, v = edge.second.second;
        int cu = ds.findSet(u), cv = ds.findSet(v);
        if (cu != cv && w <= threshold[cu] && w <= threshold[cv]) {
            ds.unite(cu, cv); // Funde os conjuntos.
            threshold[ds.findSet(cu)] = w + k / ds.getSize(ds.findSet(cu)); // Atualiza o limiar.
        }
    }

    for (int i = 0; i < V; ++i) labels[i] = ds.findSet(i); // Atribui rótulos finais.
    return labels;
}

/**
 * Converte os rótulos gerados pela segmentação em uma imagem colorida.
 * @param image Imagem original.
 * @param labels Vetor de rótulos.
 * @return Imagem segmentada com cores distintas para cada segmento.
 */
Mat colorSegments(const Mat& image, const vector<int>& labels) {
    int rows = image.rows, cols = image.cols;
    Mat segmented(rows, cols, CV_8UC3);
    unordered_map<int, Vec3b> colors; // Mapeamento de rótulos para cores.

    mt19937 rng((unsigned)random_device{}()); // Gerador de números aleatórios.
    uniform_int_distribution<int> distColor(0, 255); // Distribuição uniforme para cores.

    // Atribui cores a cada segmento.
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int label = labels[i * cols + j];
            if (colors.find(label) == colors.end()) 
                colors[label] = Vec3b(distColor(rng), distColor(rng), distColor(rng));
            segmented.at<Vec3b>(i, j) = colors[label];
        }
    }
    return segmented;
}

int main() {
    try {
        vector<string> imagePaths = { "sen.jpeg", "fror.jpg" }; // Caminhos das imagens de entrada.
        float k = 300.0f; // Parâmetro para controle de segmentação.

        for (auto& path : imagePaths) {
            cout << "Processing image: " << path << endl;
            Mat image = imread(path, IMREAD_COLOR);
            if (image.empty()) { 
                cerr << "Error loading image " << path << endl; 
                continue; 
            }

            // Pré-processamento: aplicação de um filtro Gaussiano para reduzir ruído.
            GaussianBlur(image, image, Size(5, 5), 1.5);

            int edgeCount = 0; // Contador de arestas.
            auto start = high_resolution_clock::now();
            vector<int> labels = segmentImageFH(image, k, edgeCount); // Segmenta a imagem.
            auto end = high_resolution_clock::now();

            cout << "Time: " << duration_cast<milliseconds>(end - start).count() << " ms" << endl;

            // Converte os rótulos em uma imagem segmentada colorida.
            Mat segmentedImage = colorSegments(image, labels);
            imshow("Original Image", image);
            imshow("Segmented Image", segmentedImage);
            waitKey(0);
            destroyAllWindows();
        }
        return 0;
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return -1;
    }
}