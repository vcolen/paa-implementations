#include <iostream>
#include <vector>
#include <list>
#include <queue>
#include <climits>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

class DirectedGraph2 {
    int verticesCount;
    vector<list<int>> adjacencyList;

    public:
    DirectedGraph2(int verticesCount) {
        this->verticesCount = verticesCount;
        adjacencyList.resize(verticesCount);
    }

    void addEdge(int sourceVertex, int destinationVertex) {
        adjacencyList[sourceVertex].push_back(destinationVertex);
    }

    void performDFS() {
        vector<bool>visitedVertices(verticesCount, false);
        vector<int>discoveryTime(verticesCount, -1);
        vector<int>lowestTimeToFind(verticesCount, -1);
        vector<int>parent(verticesCount, -1);
        int time = 0;
        for(int vertex = 0; vertex < verticesCount; vertex++) {
            if(!visitedVertices[vertex]) {
                DFSUtil(vertex, visitedVertices, discoveryTime, lowestTimeToFind, parent);
            }
        }
    }

    void DFSUtil(int vertex, vector<bool>& visitedVertices, vector<int>& discoveryTime, vector<int>& lowestTimeToFind, vector<int>& parent) {

    }
};