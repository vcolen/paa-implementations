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

float computeWeight(const Vec3b& c1, const Vec3b& c2) {
    float diffB = (float)c1[0] - (float)c2[0];
    float diffG = (float)c1[1] - (float)c2[1];
    float diffR = (float)c1[2] - (float)c2[2];
    return sqrt(diffB * diffB + diffG * diffG + diffR * diffR);
}

struct DisjointSets {
    vector<int> parent, rank, size;

    DisjointSets(int n) : parent(n), rank(n, 0), size(n, 1) {
        for (int i = 0; i < n; ++i) parent[i] = i;
    }

    int findSet(int u) {
        if (parent[u] != u) parent[u] = findSet(parent[u]);
        return parent[u];
    }

    void unite(int u, int v) {
        u = findSet(u), v = findSet(v);
        if (u != v) {
            if (rank[u] > rank[v]) parent[v] = u, size[u] += size[v];
            else parent[u] = v, size[v] += size[u], rank[u] == rank[v] ? rank[v]++ : 0;
        }
    }
    int getSize(int u) { return size[findSet(u)]; }
};

vector<int> segmentImageFH(const Mat& image, float k, int& edgeCount) {
    int V = image.rows * image.cols;
    vector<int> labels(V);
    for (int i = 0; i < V; ++i) labels[i] = i;

    DisjointSets ds(V);
    vector<pair<float, pair<int, int>>> edges;

    // Build graph
    for (int i = 0; i < image.rows; ++i) {
        for (int j = 0; j < image.cols; ++j) {
            Vec3b c1 = image.at<Vec3b>(i, j);
            if (i + 1 < image.rows) edges.push_back({computeWeight(c1, image.at<Vec3b>(i + 1, j)), {i * image.cols + j, (i + 1) * image.cols + j}});
            if (j + 1 < image.cols) edges.push_back({computeWeight(c1, image.at<Vec3b>(i, j + 1)), {i * image.cols + j, i * image.cols + (j + 1)}});
        }
    }

    edgeCount = edges.size();
    sort(edges.begin(), edges.end());

    vector<float> threshold(V, k);
    for (auto& edge : edges) {
        float w = edge.first;
        int u = edge.second.first, v = edge.second.second;
        int cu = ds.findSet(u), cv = ds.findSet(v);
        if (cu != cv && w <= threshold[cu] && w <= threshold[cv]) {
            ds.unite(cu, cv);
            threshold[ds.findSet(cu)] = w + k / ds.getSize(ds.findSet(cu));
        }
    }

    for (int i = 0; i < V; ++i) labels[i] = ds.findSet(i);
    return labels;
}

Mat colorSegments(const Mat& image, const vector<int>& labels) {
    int rows = image.rows, cols = image.cols;
    Mat segmented(rows, cols, CV_8UC3);
    unordered_map<int, Vec3b> colors;

    mt19937 rng((unsigned)random_device{}());
    uniform_int_distribution<int> distColor(0, 255);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int label = labels[i * cols + j];
            if (colors.find(label) == colors.end()) colors[label] = Vec3b(distColor(rng), distColor(rng), distColor(rng));
            segmented.at<Vec3b>(i, j) = colors[label];
        }
    }
    return segmented;
}

int main() {
    try {
        vector<string> imagePaths = { "sen.jpeg", "fror.jpg" };
        float k = 300.0f;

        for (auto& path : imagePaths) {
            cout << "Processing image: " << path << endl;
            Mat image = imread(path, IMREAD_COLOR);
            if (image.empty()) { cerr << "Error loading image " << path << endl; continue; }

            // Preprocess (GaussianBlur to reduce noise)
            GaussianBlur(image, image, Size(5, 5), 1.5);

            int edgeCount = 0;
            auto start = high_resolution_clock::now();
            vector<int> labels = segmentImageFH(image, k, edgeCount);
            auto end = high_resolution_clock::now();

            cout << "Time: " << duration_cast<milliseconds>(end - start).count() << " ms" << endl;

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