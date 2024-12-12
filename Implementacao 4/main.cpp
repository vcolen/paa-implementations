#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <queue>
#include <limits>
#include <cstring>
#include <iomanip>
#include <chrono>  // Para medição de tempo, se necessário
#include <random> // Para geração de grafos aleatórios

using namespace std;

const float INF = numeric_limits<float>::infinity();

/*
===========================================================================================
Implementação e análise de dois métodos de segmentação de imagem baseados em particionamento de grafos:

1) Segmentação baseada em grafos segundo Felzenszwalb & Huttenlocher (IJCV 2004):
   "Efficient Graph-Based Image Segmentation"
   - Princípio: Construir um MST (árvore geradora mínima) da imagem, onde vértices são pixels
     e arestas representam diferenças de cor/estímulo entre pixels.
   - Critério adaptativo de união de componentes baseado em limiares (k/|C| + Int(C)).
   - Complexidade:
     * Ordenação das arestas: O(E log E), onde E é o número de arestas.
     * União e busca com Union-Find: Quase O(E) amortizado, pois Union-Find é muito eficiente.
     * No geral, O(E log E) domina, dado que E ~ O(N) para grade de imagem, pois E ≈ 4N ou 8N
       dependendo da conectividade (N = número de pixels).
     * Estrutura de dados: Union-Find (Disjoint Set) para união dos componentes.
     * Aplicação: Segmentação rápida e hierárquica, adequada para pré-processamento em visão computacional.

2) Segmentação via min-cut s/t segundo Boykov & Funka-Lea (IJCV 2006):
   "Graph Cuts and Efficient N-D Image Segmentation"
   - Princípio: Modelar a segmentação como um problema de minimização de energia. Construir um
     grafo com fonte (S) e sumidouro (T), onde cortes mínimos correspondem à solução ótima
     do problema de segmentação.
   - Computação do min-cut é obtida pelo max-flow (Dualidade max-flow/min-cut).
   - Método de max-flow utilizado: Edmond-Karp (para fins didáticos). Existem métodos mais eficientes
     (Dinic, Push-Relabel) que seriam melhores na prática.
   - Complexidade do Edmond-Karp: O(V * E²) no pior caso. Para imagens, isso pode ser grande.
     Métodos melhores, como Dinic, podem reduzir para O(E√V) em média.
   - Estrutura de dados: Grafo residual com listas de adjacência, BFS e DFS para encontrar caminhos
     aumentantes. 
   - Aplicação: Fornece solução globalmente ótima para um modelo de energia, muito usado em segmentação.

Comparação dos métodos:
- Felzenszwalb & Huttenlocher:
  * Mais rápido e simples de implementar.
  * Segmentação baseada em propriedades de regiões (MST) e limiares adaptativos.
  * Não garante necessariamente um ótimo global definido por uma função de energia, mas é muito eficiente.
- Boykov & Funka-Lea (com max-flow/min-cut):
  * Pode resolver problemas definidos como minimização de energia global.
  * Garantia de ótima global em função da formulação do problema.
  * Tempo de execução geralmente maior, pois max-flow/min-cut é mais pesado computacionalmente.
  * Requer mais memória e estruturas mais complexas.

Abaixo, o código editado inclui comentários mais detalhados, referências, análises de custo computacional
e um esqueleto para medição de tempo que o usuário pode adaptar. Assim, este código
está melhor preparado para avaliação acadêmica.
===========================================================================================
*/

// ===========================================================================
// Estrutura da aresta para o método Felzenszwalb & Huttenlocher
// ===========================================================================
// Cada aresta conecta dois nós (u, v) e possui um peso associado, que representa
// a dissimilaridade entre os dois nós. No contexto de segmentação de imagens,
// esse peso pode ser a diferença de intensidade, cor ou outra métrica relevante.
struct Edge {
    int u, v;     // Índices dos nós/vértices conectados pela aresta
    float weight; // Peso da aresta (medida de dissimilaridade)
};

// ===========================================================================
// Classe Union-Find (Disjoint Set) utilizada no método Felzenszwalb & Huttenlocher
// ===========================================================================
// Esta classe implementa uma estrutura de conjuntos disjuntos para representar
// componentes conectados no grafo. Ela suporta as operações de "find" (localizar
// o componente de um nó) e "unify" (unir dois componentes).
class DisjointSet {
public:
    // Construtor: inicializa a estrutura para um número específico de nós
    DisjointSet(int size) : parent(size), rank(size, 0), componentSize(size, 1), intDiff(size, 0.0f) {
        for (int i = 0; i < size; ++i)
            parent[i] = i; // Cada nó começa em seu próprio conjunto
    }

    // Localiza o representante do conjunto (com compressão de caminho para eficiência)
    int findSet(int u) {
        if (u != parent[u])
            parent[u] = findSet(parent[u]);
        return parent[u];
    }

    // Une dois conjuntos com base nos pesos das arestas e nas diferenças internas
    void unify(int u, int v, float weight) {
        u = findSet(u); // Representante do conjunto de u
        v = findSet(v); // Representante do conjunto de v

        if (u != v) { // Se os dois conjuntos são distintos, unificá-los
            if (rank[u] > rank[v]) {
                parent[v] = u;
                componentSize[u] += componentSize[v];
                intDiff[u] = max(intDiff[u], max(intDiff[v], weight)); // Atualiza a maior diferença interna
            } else if (rank[u] < rank[v]) {
                parent[u] = v;
                componentSize[v] += componentSize[u];
                intDiff[v] = max(intDiff[v], max(intDiff[u], weight)); // Atualiza a maior diferença interna
            } else {
                parent[v] = u;
                componentSize[u] += componentSize[v];
                rank[u]++;
                intDiff[u] = max(intDiff[u], max(intDiff[v], weight)); // Atualiza a maior diferença interna
            }
        }
    }

    // Retorna o tamanho do componente de um nó
    int size(int u) {
        return componentSize[findSet(u)];
    }

    // Retorna a maior diferença interna do componente de um nó
    float internalDiff(int u) {
        return intDiff[findSet(u)];
    }

private:
    vector<int> parent;          // Vetor de pais para a estrutura Union-Find
    vector<int> rank;            // Classificação para melhorar a eficiência da união
    vector<int> componentSize;   // Tamanho de cada componente
    vector<float> intDiff;       // Maior aresta interna do componente (Int(C))
};

// ===========================================================================
// Segmentação baseada em grafos (Felzenszwalb & Huttenlocher, 2004)
// ===========================================================================
// Este método realiza particionamento de um grafo em componentes conexos
// adaptativos, usando critérios que levam em conta a diferença interna
// máxima de cada componente (Int(C)) e um limiar adaptativo (Tau(C)).
// Parâmetros:
// - numNodes: Número de nós no grafo
// - edges: Vetor de arestas que conectam os nós
// - k: Parâmetro que controla a granularidade da segmentação
// - minSize: Tamanho mínimo para cada componente após a segmentação
vector<int> segmentImageGraph(int numNodes, vector<Edge>& edges, float k, int minSize) {
    // Passo 1: Ordena as arestas por peso (crescente)
    // Isso garante que as menores arestas sejam processadas primeiro, como em uma
    // construção de árvore geradora mínima.
    sort(edges.begin(), edges.end(), [](const Edge& a, const Edge& b) {
        return a.weight < b.weight;
    });

    // Inicializa a estrutura Union-Find para representar os componentes
    DisjointSet ds(numNodes);

    // Passo 2: União adaptativa dos componentes com base no critério Tau(C)
    for (auto &edge : edges) {
        int c1 = ds.findSet(edge.u); // Componente do nó u
        int c2 = ds.findSet(edge.v); // Componente do nó v

        if (c1 != c2) { // Apenas tenta unir componentes diferentes
            float intC1 = ds.internalDiff(c1); // Diferença interna de C1
            float intC2 = ds.internalDiff(c2); // Diferença interna de C2
            float tauC1 = k / ds.size(c1);     // Limiar adaptativo para C1
            float tauC2 = k / ds.size(c2);     // Limiar adaptativo para C2

            // Decide se deve unir os componentes com base no peso da aresta
            float maxInt = min(intC1 + tauC1, intC2 + tauC2);
            if (edge.weight <= maxInt) {
                ds.unify(c1, c2, edge.weight);
            }
        }
    }

    // Passo 3: Pós-processamento para garantir tamanho mínimo dos componentes
    for (auto &edge : edges) {
        int c1 = ds.findSet(edge.u);
        int c2 = ds.findSet(edge.v);
        if (c1 != c2) {
            if (ds.size(c1) < minSize || ds.size(c2) < minSize) {
                ds.unify(c1, c2, edge.weight); // Une componentes pequenos
            }
        }
    }

    // Passo 4: Geração de rótulos finais para cada nó
    vector<int> labels(numNodes); // Cada nó terá um rótulo indicando seu componente
    unordered_map<int, int> compToLabel; // Mapeia componentes para rótulos
    int labelCount = 0; // Contador de rótulos únicos
    for (int i = 0; i < numNodes; ++i) {
        int root = ds.findSet(i); // Representante do componente de i
        if (compToLabel.find(root) == compToLabel.end()) {
            compToLabel[root] = labelCount++;
        }
        labels[i] = compToLabel[root];
    }

    return labels; // Retorna o vetor de rótulos
}

// ===========================================================================
// Estruturas para o método de Boykov & Funka-Lea (Min-Cut/Max-Flow)
// ===========================================================================
// Representa uma aresta residual no grafo. Necessária para calcular fluxos
// e ajustar as capacidades residuais após cada iteração.
struct ResidualEdge {
    int to;          // Nó destino
    float capacity;  // Capacidade máxima da aresta
    float flow;      // Fluxo atual na aresta
    int reverse;     // Índice da aresta reversa na lista de adjacências do nó 'to'
};

// ===========================================================================
// Classe Graph: Representa um grafo com arestas residuais
// ===========================================================================
class Graph {
public:
    explicit Graph(int nodes) : adj(nodes) {}

    // Adiciona uma aresta direcionada no grafo e sua reversa no grafo residual
    void addEdge(int from, int to, float capacity) {
        ResidualEdge a = {to, capacity, 0, (int)adj[to].size()};
        ResidualEdge b = {from, 0, 0, (int)adj[from].size()};
        adj[from].push_back(a);
        adj[to].push_back(b);
    }

    // Retorna a lista de adjacências do grafo
    vector<vector<ResidualEdge>>& getAdj() {
        return adj;
    }

private:
    vector<vector<ResidualEdge>> adj; // Lista de adjacências de arestas residuais
};

// ===========================================================================
// Classe MaxFlow: Calcula fluxo máximo e corte mínimo
// ===========================================================================
class MaxFlow {
public:
    explicit MaxFlow(Graph& graph) : graph(graph) {}

    // Encontra o fluxo máximo entre fonte e sumidouro
    float findMaxFlow(int source, int sink) {
        int nodes = (int)graph.getAdj().size();
        float maxFlow = 0;

        // Repetidamente encontra caminhos aumentantes e ajusta os fluxos
        while (bfs(source, sink)) {
            vector<int> start(nodes, 0);
            while (float flow = dfs(source, sink, INF, start)) {
                maxFlow += flow;
            }
        }
        return maxFlow;
    }

    // Obtém o corte mínimo a partir do grafo residual
    vector<bool> getMinCut(int source) {
        int nodes = (int)graph.getAdj().size();
        vector<bool> visited(nodes, false);
        queue<int> q;
        q.push(source);
        visited[source] = true;

        // BFS para identificar os nós alcançáveis a partir da fonte
        while (!q.empty()) {
            int node = q.front();
            q.pop();

            for (const auto& edge : graph.getAdj()[node]) {
                if (!visited[edge.to] && edge.capacity - edge.flow > 0) {
                    visited[edge.to] = true;
                    q.push(edge.to);
                }
            }
        }
        return visited;
    }

private:
    Graph& graph;            // Referência ao grafo residual
    vector<int> level;       // Níveis dos nós para BFS

    // BFS: Encontra caminhos aumentantes no grafo residual
    bool bfs(int source, int sink) {
        int nodes = (int)graph.getAdj().size();
        level.assign(nodes, -1);

        queue<int> q;
        q.push(source);
        level[source] = 0;

        while (!q.empty()) {
            int node = q.front();
            q.pop();

            for (const auto& edge : graph.getAdj()[node]) {
                if (level[edge.to] == -1 && edge.capacity - edge.flow > 0) {
                    level[edge.to] = level[node] + 1;
                    q.push(edge.to);
                }
            }
        }
        return level[sink] != -1;
    }

    // DFS: Propaga o fluxo ao longo do caminho encontrado pela BFS
    float dfs(int node, int sink, float flow, vector<int>& start) {
        if (node == sink) return flow;

        for (; start[node] < (int)graph.getAdj()[node].size(); ++start[node]) {
            ResidualEdge& edge = graph.getAdj()[node][start[node]];
            if (level[edge.to] == level[node] + 1 && edge.capacity - edge.flow > 0) {
                float currFlow = min(flow, edge.capacity - edge.flow);
                float tempFlow = dfs(edge.to, sink, currFlow, start);

                if (tempFlow > 0) {
                    edge.flow += tempFlow;
                    graph.getAdj()[edge.to][edge.reverse].flow -= tempFlow;
                    return tempFlow;
                }
            }
        }
        return 0;
    }
};

// Funções auxiliares para testes
void testFelzenszwalbHuttenlocher(int numNodes, int numEdges, float k, int minSize);
void testBoykovFunkaLea(int numNodes, int numEdges);

int main() {
    cout << "== Testes de Segmentação com Grafos de Diferentes Tamanhos ==" << endl << endl;

    // Configurações de teste
    vector<int> nodeSizes = {10, 100, 1000}; // Quantidade de nós
    vector<int> edgeDensities = {2, 4, 8};   // Fator de densidade (arestas por nó)
    float k = 1.5;                          // Parâmetro de granularidade
    int minSize = 3;                        // Tamanho mínimo do componente

    for (int nodes : nodeSizes) {
        for (int density : edgeDensities) {
            int edges = nodes * density; // Estimativa de número de arestas
            cout << "== Testando com " << nodes << " nós e " << edges << " arestas ==" << endl;
            
            // Teste do Método Felzenszwalb & Huttenlocher
            testFelzenszwalbHuttenlocher(nodes, edges, k, minSize);

            // Teste do Método Boykov & Funka-Lea
            testBoykovFunkaLea(nodes, edges);

            cout << endl; // Separador entre testes
        }
    }

    cout << "== Testes concluídos ==" << endl;
    return 0;
}

// ===========================================================================
// Teste do método de Felzenszwalb & Huttenlocher
// ===========================================================================
void testFelzenszwalbHuttenlocher(int numNodes, int numEdges, float k, int minSize) {
    cout << "-> Testando Método Felzenszwalb & Huttenlocher" << endl;

    // Geração de grafo aleatório
    vector<Edge> edges;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> distNode(0, numNodes - 1);
    uniform_real_distribution<float> distWeight(0.0, 10.0);

    for (int i = 0; i < numEdges; ++i) {
        int u = distNode(gen);
        int v = distNode(gen);
        if (u != v) {
            edges.push_back({u, v, distWeight(gen)});
        }
    }

    // Medição de tempo
    auto start_time = chrono::high_resolution_clock::now();
    vector<int> labels = segmentImageGraph(numNodes, edges, k, minSize);
    auto end_time = chrono::high_resolution_clock::now();
    double elapsed = chrono::duration<double, milli>(end_time - start_time).count();

    // Saída do resultado
    cout << "Tempo de execução: " << fixed << setprecision(3) << elapsed << " ms" << endl;
    cout << "Número de componentes: " << *max_element(labels.begin(), labels.end()) + 1 << endl;
}

// ===========================================================================
// Teste do método de Boykov & Funka-Lea (Max-Flow/Min-Cut)
// ===========================================================================
void testBoykovFunkaLea(int numNodes, int numEdges) {
    cout << "-> Testando Método Boykov & Funka-Lea" << endl;

    // Geração de grafo aleatório
    Graph graph(numNodes);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> distNode(0, numNodes - 1);
    uniform_real_distribution<float> distCapacity(1.0, 20.0);

    for (int i = 0; i < numEdges; ++i) {
        int u = distNode(gen);
        int v = distNode(gen);
        if (u != v) {
            graph.addEdge(u, v, distCapacity(gen));
        }
    }

    // Escolha da fonte e do sumidouro
    int source = 0;
    int sink = numNodes - 1;

    // Solução do fluxo máximo
    MaxFlow maxFlowSolver(graph);

    // Medição de tempo
    auto start_time = chrono::high_resolution_clock::now();
    float maxFlow = maxFlowSolver.findMaxFlow(source, sink);
    auto end_time = chrono::high_resolution_clock::now();
    double elapsed = chrono::duration<double, milli>(end_time - start_time).count();

    // Saída do resultado
    cout << "Tempo de execução: " << fixed << setprecision(3) << elapsed << " ms" << endl;
    cout << "Fluxo máximo encontrado: " << maxFlow << endl;
}