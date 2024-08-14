#include <iostream>
#include <vector>
#include <list>
#include <queue>
#include <climits>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

// Classe para representar um grafo direcionado usando lista de adjacência
class DirectedGraph {
    int numVertices; // Número de vértices
    vector<list<int>> adjacencyList; // Lista de adjacência

    // Função auxiliar para a busca em profundidade
    void DFSUtil(int vertex, vector<bool> &visited, vector<int> &discoveryTime, vector<int> &lowTime, vector<int> &parent, int &time) {
        visited[vertex] = true;
        discoveryTime[vertex] = lowTime[vertex] = ++time;

        for (int adjacentVertex : adjacencyList[vertex]) {
            if (!visited[adjacentVertex]) {
                parent[adjacentVertex] = vertex;
                cout << "Aresta (" << vertex << " -> " << adjacentVertex << "): Árvore\n";
                DFSUtil(adjacentVertex, visited, discoveryTime, lowTime, parent, time);
                lowTime[vertex] = min(lowTime[vertex], lowTime[adjacentVertex]);

                if (lowTime[adjacentVertex] > discoveryTime[vertex])
                    cout << "Aresta (" << vertex << " -> " << adjacentVertex << "): Avanço\n";
            } else if (adjacentVertex != parent[vertex]) {
                if (discoveryTime[adjacentVertex] < discoveryTime[vertex])
                    cout << "Aresta (" << vertex << " -> " << adjacentVertex << "): Retorno\n";
                else
                    cout << "Aresta (" << vertex << " -> " << adjacentVertex << "): Cruzamento\n";
                lowTime[vertex] = min(lowTime[vertex], discoveryTime[adjacentVertex]);
            }
        }
    }

public:
    // Construtor
    DirectedGraph(int numVertices) {
        this->numVertices = numVertices;
        adjacencyList.resize(numVertices);
    }

    // Função para adicionar uma aresta ao grafo
    void addEdge(int sourceVertex, int destinationVertex) {
        adjacencyList[sourceVertex].push_back(destinationVertex);
    }

    // Função para realizar a busca em profundidade e rotular arestas
    void performDFS() {
        vector<bool> visited(numVertices, false);
        vector<int> discoveryTime(numVertices, -1);
        vector<int> lowTime(numVertices, -1);
        vector<int> parent(numVertices, -1);
        int time = 0;

        for (int vertex = 0; vertex < numVertices; vertex++)
            if (!visited[vertex])
                DFSUtil(vertex, visited, discoveryTime, lowTime, parent, time);
    }
};

// Classe para representar um grafo não-direcionado usando matriz de adjacência
class UndirectedGraph {
    int numVertices; // Número de vértices
    vector<vector<int>> adjacencyMatrix; // Matriz de adjacência

public:
    // Construtor
    UndirectedGraph(int numVertices) {
        this->numVertices = numVertices;
        adjacencyMatrix.resize(numVertices, vector<int>(numVertices, 0));
    }

    // Função para adicionar uma aresta ao grafo
    void addEdge(int vertex1, int vertex2) {
        adjacencyMatrix[vertex1][vertex2] = 1;
        adjacencyMatrix[vertex2][vertex1] = 1;
    }

    // Função para realizar a busca em largura e calcular a excentricidade de um vértice
    int calculateExcentricity(int startVertex) {
        vector<int> distance(numVertices, INT_MAX);
        queue<int> bfsQueue;

        distance[startVertex] = 0;
        bfsQueue.push(startVertex);

        while (!bfsQueue.empty()) {
            int currentVertex = bfsQueue.front();
            bfsQueue.pop();

            for (int i = 0; i < numVertices; i++) {
                if (adjacencyMatrix[currentVertex][i] && distance[i] == INT_MAX) {
                    distance[i] = distance[currentVertex] + 1;
                    bfsQueue.push(i);
                }
            }
        }

        // Encontrar a excentricidade (maior distância)
        int excentricity = 0;
        for (int i = 0; i < numVertices; i++) {
            if (distance[i] != INT_MAX && distance[i] > excentricity)
                excentricity = distance[i];
        }

        return excentricity;
    }

    // Função para obter o número de vértices
    int getNumVertices() const {
        return numVertices;
    }

    // Função para verificar a existência de uma aresta
    bool hasEdge(int vertex1, int vertex2) const {
        return adjacencyMatrix[vertex1][vertex2] != 0;
    }
};

// Função para criar um grafo direcionado aleatório
DirectedGraph createRandomDirectedGraph(int maxVertices = 10) {
    srand(time(0));
    int numVertices = rand() % maxVertices + 1; // Número de vértices entre 1 e maxVertices
    DirectedGraph graph(numVertices);

    int numEdges = rand() % (numVertices * (numVertices - 1)); // Número de arestas entre 0 e numVertices*(numVertices-1)
    for (int i = 0; i < numEdges; i++) {
        int source = rand() % numVertices;
        int destination = rand() % numVertices;
        if (source != destination) { // Evitar loops
            graph.addEdge(source, destination);
        }
    }

    return graph;
}

// Função para criar um grafo não-direcionado aleatório
UndirectedGraph createRandomUndirectedGraph(int maxVertices = 10) {
    srand(time(0));
    int numVertices = rand() % maxVertices + 1; // Número de vértices entre 1 e maxVertices
    UndirectedGraph graph(numVertices);

    int numEdges = rand() % (numVertices * (numVertices - 1) / 2); // Número de arestas entre 0 e numVertices*(numVertices-1)/2
    for (int i = 0; i < numEdges; i++) {
        int vertex1 = rand() % numVertices;
        int vertex2 = rand() % numVertices;
        if (vertex1 != vertex2 && !graph.hasEdge(vertex1, vertex2)) { // Evitar loops e duplicação de arestas
            graph.addEdge(vertex1, vertex2);
        }
    }

    return graph;
}

int main() {
    // Criar e analisar um grafo direcionado aleatório
    DirectedGraph directedGraph = createRandomDirectedGraph();
    cout << "Rotulação das arestas no grafo direcionado aleatório:\n";
    directedGraph.performDFS();

    // Criar e analisar um grafo não-direcionado aleatório
    UndirectedGraph undirectedGraph = createRandomUndirectedGraph();
    cout << "\nExcentricidade de cada vértice no grafo não-direcionado aleatório:\n";
    for (int vertex = 0; vertex < undirectedGraph.getNumVertices(); vertex++) {
        int excentricity = undirectedGraph.calculateExcentricity(vertex);
        cout << "Vértice " << vertex << ": Excentricidade = " << excentricity << endl;
    }

    return 0;
}
