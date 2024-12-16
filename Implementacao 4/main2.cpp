#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <queue>
#include <limits>
#include <chrono>

using namespace cv;
using namespace std;
using namespace chrono;

struct Edge {
    int to;
    float capacity;
    int rev;
};

class Graph {
public:
    Graph(int n) : adj(n), N(n) {}

    void addEdge(int u, int v, float capacity) {
        Edge a = {v, capacity, (int)adj[v].size()};
        Edge b = {u, 0.0f, (int)adj[u].size()};
        adj[u].push_back(a);
        adj[v].push_back(b);
        edgeCount++;
    }

    float maxFlow(int s, int t) {
        source = s; sink = t;
        float flow = 0.f;
        while (bfs()) {
            start.assign(N, 0);
            while (float f = sendFlow(source, numeric_limits<float>::infinity())) {
                flow += f;
            }
        }
        return flow;
    }

    vector<bool> getMinCut() {
        vector<bool> cut(N, false);
        for (int i = 0; i < N; i++) {
            if (level[i] >= 0) cut[i] = true;
        }
        return cut;
    }

    int getEdgeCount() const { return edgeCount; }

private:
    vector<vector<Edge>> adj;
    vector<int> level, start;
    int N, source, sink;
    int edgeCount = 0;

    bool bfs() {
        level.assign(N, -1);
        level[source] = 0;
        queue<int> q;
        q.push(source);
        while (!q.empty()) {
            int u = q.front(); q.pop();
            for (auto &e : adj[u]) {
                if (level[e.to] < 0 && e.capacity > 0) {
                    level[e.to] = level[u] + 1;
                    q.push(e.to);
                }
            }
        }
        return level[sink] >= 0;
    }

    float sendFlow(int u, float flow) {
        if (u == sink) return flow;
        for (; start[u] < (int)adj[u].size(); start[u]++) {
            Edge &e = adj[u][start[u]];
            if (level[e.to] == level[u] + 1 && e.capacity > 0) {
                float curr_flow = std::min(flow, e.capacity);
                float temp_flow = sendFlow(e.to, curr_flow);
                if (temp_flow > 0) {
                    e.capacity -= temp_flow;
                    adj[e.to][e.rev].capacity += temp_flow;
                    return temp_flow;
                }
            }
        }
        return 0;
    }
};

inline float safeLog(float x) {
    if (x < 1e-10f) x = 1e-10f;
    return -log(x);
}

void buildHistograms(const Mat &gray, const Mat &maskObj, const Mat &maskBkg,
                     vector<int> &objHist, vector<int> &bkgHist,
                     int &objCount, int &bkgCount) {
    objHist.assign(256, 0);
    bkgHist.assign(256, 0);
    objCount = 0; bkgCount = 0;
    for (int r = 0; r < gray.rows; r++) {
        for (int c = 0; c < gray.cols; c++) {
            int val = (int)gray.at<uchar>(r, c);
            if (maskObj.at<uchar>(r, c) > 128) {
                objHist[val]++;
                objCount++;
            }
            if (maskBkg.at<uchar>(r, c) > 128) {
                bkgHist[val]++;
                bkgCount++;
            }
        }
    }
}

Mat segmentImageThresholdBased(const Mat &image) {
    Mat gray;
    cvtColor(image, gray, COLOR_BGR2GRAY);

    double threshVal = 80;
    Mat maskObj, maskBkg;
    threshold(gray, maskObj, threshVal, 255, THRESH_BINARY);
    maskBkg = Mat::ones(gray.size(), CV_8UC1) * 255 - maskObj;

    vector<int> objHist, bkgHist;
    int objCount, bkgCount;
    buildHistograms(gray, maskObj, maskBkg, objHist, bkgHist, objCount, bkgCount);

    auto D_obj = [&](int val) {
        float p = (objCount > 0) ? ((float)objHist[val] / (float)objCount) : 1e-10f;
        return safeLog(p);
    };
    auto D_bkg = [&](int val) {
        float p = (bkgCount > 0) ? ((float)bkgHist[val] / (float)bkgCount) : 1e-10f;
        return safeLog(p);
    };

    int rows = gray.rows, cols = gray.cols;
    int numPixels = rows * cols;
    float sigma = 10.0f;
    auto boundaryWeight = [&](int u, int v) {
        int r1 = u / cols, c1 = u % cols;
        int r2 = v / cols, c2 = v % cols;
        float diff = (float)gray.at<uchar>(r1, c1) - (float)gray.at<uchar>(r2, c2);
        float diff2 = diff * diff;
        return exp(-diff2 / (2 * sigma * sigma));
    };

    float INF = 1e9f;
    int source = numPixels, sink = numPixels + 1;
    Graph g(numPixels + 2);

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int u = r * cols + c;
            if (c + 1 < cols) {
                int v = r * cols + (c + 1);
                float w = boundaryWeight(u, v);
                g.addEdge(u, v, w);
                g.addEdge(v, u, w);
            }
            if (r + 1 < rows) {
                int v = (r + 1) * cols + c;
                float w = boundaryWeight(u, v);
                g.addEdge(u, v, w);
                g.addEdge(v, u, w);
            }
        }
    }

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int u = r * cols + c;
            int val = (int)gray.at<uchar>(r, c);
            bool objSeed = (maskObj.at<uchar>(r, c) > 128);
            bool bkgSeed = (maskBkg.at<uchar>(r, c) > 128);
            if (objSeed) {
                g.addEdge(source, u, INF);
            } else if (bkgSeed) {
                g.addEdge(u, sink, INF);
            } else {
                g.addEdge(source, u, D_bkg(val));
                g.addEdge(u, sink, D_obj(val));
            }
        }
    }

    auto start = high_resolution_clock::now();
    g.maxFlow(source, sink);
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);

    cout << "Partitioning time: " << duration.count() << " ms" << endl;
    cout << "Number of nodes: " << numPixels + 2 << endl;
    cout << "Number of edges: " << g.getEdgeCount() << endl;

    vector<bool> minCut = g.getMinCut();
    Mat seg(rows, cols, CV_8UC3, Scalar(255, 0, 0));
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int u = r * cols + c;
            if (minCut[u]) {
                seg.at<Vec3b>(r, c) = Vec3b(0, 0, 255);
            }
        }
    }

    return seg;
}

int main() {
    vector<string> imagePaths = {"sen.jpeg", "fror.jpg"}; // Adicione os caminhos das imagens aqui

    for (const auto& imagePath : imagePaths) {
        Mat image = imread(imagePath, IMREAD_COLOR);
        if (image.empty()) {
            cerr << "Error loading image: " << imagePath << endl;
            continue;
        }

        Mat segmented = segmentImageThresholdBased(image);
        imshow("Original - " + imagePath, image);
        imshow("Segmented - " + imagePath, segmented);
        waitKey(0); // Aguarda o usuário pressionar uma tecla antes de continuar
    }

    return 0;
}