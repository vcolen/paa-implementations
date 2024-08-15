#include <iostream>
#include <map>
#include <fstream>
#include <cstdlib>

using namespace std;

class SearchAlgorithms
{

public:
    static bool SimpleLinearSearch(int *array, int sizeOfArray, int x, int &operationsCount)
    {
        operationsCount += 2; // int i; i = 0;
        for (int i = 0; i < sizeOfArray; i++)
        {
            operationsCount++; // i < sizeOfArray
            if (x == array[i])
            {
                operationsCount++; // return
                return true;
            }
            operationsCount++; // if (x == array[i])
        }
        operationsCount += 2; // i < sizeOfArray; return false
        return false;
    }

    static bool BinarySearch(int *array, int left, int right, int x, int &operationsCount)
    {
        while (left <= right)
        {
            operationsCount++; // while (left <= right)
            int mid = left + (right - left) / 2;
            operationsCount += 4; // int mid = left + (right - left) / 2;
            operationsCount++;    //  if (array[mid] == x)
            if (array[mid] == x)
            {
                operationsCount++; // return true
                return true;
            }
            operationsCount++; // if (array[mid] < x)
            if (array[mid] < x)
            {
                operationsCount += 2; // left = mid + 1
                left = mid + 1;
            }
            else
            {
                operationsCount += 2; // right = mid - 1;
                right = mid - 1;
            }
        }
        operationsCount += 2; // while (left <= right); return false;
        return false;
    }

    static bool SentinelLinearSearch(int *array, int lastIndex, int x, int &operationsCount)
    {
        int i = 0;
        operationsCount += 2; // int i = 0
        array[lastIndex] = x; 
        operationsCount += 2; // array[lastIndex] = x;
        while (array[i] != x) {
            i++;
            operationsCount += 2; //  while (array[i] != x); i++;
        }
        operationsCount += 3; // while (array[i] != x); return i != lastIndex
        return i != lastIndex;
    }
};

void TestSimpleLinearSearch(int *megaArray, std::map<std::string, int> &operationsCountByAlgorithm);
void TestBinarySearch(int *megaArray, std::map<std::string, int> &operationsCountByAlgorithm);
void TestSentinelLinearSearch(int *megaArray, std::map<std::string, int> &operationsCountByAlgorithm);
void ExportToCsv(const std::map<std::string, int>& operationsCountByAlgorithm);
void PlotCharts();

int main() {
    std::map<std::string, int> operationsCountByAlgorithm;
    int megaArray[1000000];
    populateMegaArray(megaArray);

    TestSimpleLinearSearch(megaArray, operationsCountByAlgorithm);
    TestBinarySearch(megaArray, operationsCountByAlgorithm);
    TestSentinelLinearSearch(megaArray, operationsCountByAlgorithm);

    ExportToCsv(operationsCountByAlgorithm);
    PlotCharts();

    return 0;
}

void TestSimpleLinearSearch(int *megaArray, std::map<std::string, int> &operationsCountByAlgorithm) {
    int operationsCount = 0;

    // Element at the beginning
    operationsCount = 0;
    SearchAlgorithms::SimpleLinearSearch(megaArray, 1000000, megaArray[0], operationsCount);
    operationsCountByAlgorithm["SimpleLinearSearch_Beginning"] = operationsCount;
    std::cout << "SimpleLinearSearch (Element at Beginning) - Operations: " << operationsCount << std::endl;

    // Element in the middle
    operationsCount = 0;
    SearchAlgorithms::SimpleLinearSearch(megaArray, 1000000, megaArray[500000], operationsCount);
    operationsCountByAlgorithm["SimpleLinearSearch_Middle"] = operationsCount;
    std::cout << "SimpleLinearSearch (Element in Middle) - Operations: " << operationsCount << std::endl;

    // Element at the end
    operationsCount = 0;
    SearchAlgorithms::SimpleLinearSearch(megaArray, 1000000, megaArray[999999], operationsCount);
    operationsCountByAlgorithm["SimpleLinearSearch_End"] = operationsCount;
    std::cout << "SimpleLinearSearch (Element at End) - Operations: " << operationsCount << std::endl;

    // Element not present
    operationsCount = 0;
    SearchAlgorithms::SimpleLinearSearch(megaArray, 1000000, 1000001, operationsCount);
    operationsCountByAlgorithm["SimpleLinearSearch_NotFound"] = operationsCount;
    std::cout << "SimpleLinearSearch (Element Not Present) - Operations: " << operationsCount << std::endl;
}

void TestBinarySearch(int *megaArray, std::map<std::string, int> &operationsCountByAlgorithm) {
    int operationsCount = 0;

    // Element at the beginning
    operationsCount = 0;
    SearchAlgorithms::BinarySearch(megaArray, 0, 1000000, megaArray[0], operationsCount);
    operationsCountByAlgorithm["BinarySearch_Beginning"] = operationsCount;
    std::cout << "BinarySearch (Element at Beginning) - Operations: " << operationsCount << std::endl;

    // Element in the middle
    operationsCount = 0;
    SearchAlgorithms::BinarySearch(megaArray, 0, 1000000, megaArray[500000], operationsCount);
    operationsCountByAlgorithm["BinarySearch_Middle"] = operationsCount;
    std::cout << "BinarySearch (Element in Middle) - Operations: " << operationsCount << std::endl;

    // Element at the end
    operationsCount = 0;
    SearchAlgorithms::BinarySearch(megaArray, 0, 1000000, megaArray[999999], operationsCount);
    operationsCountByAlgorithm["BinarySearch_End"] = operationsCount;
    std::cout << "BinarySearch (Element at End) - Operations: " << operationsCount << std::endl;

    // Element not present
    operationsCount = 0;
    SearchAlgorithms::BinarySearch(megaArray, 0, 1000000, 1000001, operationsCount);
    operationsCountByAlgorithm["BinarySearch_NotFound"] = operationsCount;
    std::cout << "BinarySearch (Element Not Present) - Operations: " << operationsCount << std::endl;
}

void TestSentinelLinearSearch(int *megaArray, std::map<std::string, int> &operationsCountByAlgorithm) {
    int operationsCount = 0;

    // Test 1: Element at the beginning
    operationsCount = 0;
    SearchAlgorithms::SentinelLinearSearch(megaArray, 999999, megaArray[0], operationsCount);
    operationsCountByAlgorithm["SentinelLinearSearch_Beginning"] = operationsCount;
    std::cout << "SentinelLinearSearch (Element at Beginning) - Operations: " << operationsCount << std::endl;

    // Element in the middle
    operationsCount = 0;
    SearchAlgorithms::SentinelLinearSearch(megaArray, 999999, megaArray[500000], operationsCount);
    operationsCountByAlgorithm["SentinelLinearSearch_Middle"] = operationsCount;
    std::cout << "SentinelLinearSearch (Element in Middle) - Operations: " << operationsCount << std::endl;

    // Element at the end
    operationsCount = 0;
    SearchAlgorithms::SentinelLinearSearch(megaArray, 999999, megaArray[999999], operationsCount);
    operationsCountByAlgorithm["SentinelLinearSearch_End"] = operationsCount;
    std::cout << "SentinelLinearSearch (Element at End) - Operations: " << operationsCount << std::endl;

    // Element not present
    operationsCount = 0;
    SearchAlgorithms::SentinelLinearSearch(megaArray, 999999, 1000001, operationsCount);
    operationsCountByAlgorithm["SentinelLinearSearch_NotFound"] = operationsCount;
    std::cout << "SentinelLinearSearch (Element Not Present) - Operations: " << operationsCount << std::endl;
}

void populateMegaArray(int *megaArray)
{
    for (int i = 0; i < 1000000; i++)
    {
        megaArray[i] = i;
    }
}
void ExportToCsv(const std::map<std::string, int>& operationsCountByAlgorithm)
{
    std::ofstream file("operations_count.csv");

    if (!file.is_open()) {
        std::cerr << "Failed to open the file for writing." << std::endl;
        return;
    }

    // Write the CSV header
    file << "Algorithm,Operations\n";

    // Iterate over the map and write each entry to the CSV
    for (const auto& entry : operationsCountByAlgorithm) {
        file << entry.first << "," << entry.second << "\n";
    }

    file.close();
    std::cout << "Data successfully exported to operations_count.csv" << std::endl;
}

void PlotCharts()
{
    system("python3 plot_operations.py"); // On Linux or MacOS
    // system("python plot_operations.py");  // On Windows
}
