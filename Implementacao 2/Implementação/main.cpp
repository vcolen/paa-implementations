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

void populateMegaArray(int *megaArray)
{
    for (int i = 0; i < 1000000; i++)
    {
        megaArray[i] = i;
    }
}
void ExportToCsv(int operationsCount)
{
    std::ofstream file("operations_count.csv");
    file << "Algorithm,Operations\n";
    file << "SimpleLinearSearch," << operationsCount << "\n";
    file << "BinarySearch," << operationsCount << "\n";
    file << "SentinelLinearSearch," << operationsCount << "\n";
    file.close();
}

void PlotCharts()
{
    system("python3 plot_operations.py"); // On Linux or MacOS
    // system("python plot_operations.py");  // On Windows
}

int main()
{
    map<string, int> operationsCountByAlgorithm;
    int megaArray[1000000];
    populateMegaArray(megaArray);

    int operationsCount = 0;
    SearchAlgorithms::SimpleLinearSearch(megaArray, 1000000, 1000001, operationsCount);
    operationsCountByAlgorithm["SimpleLinearSearch"] = operationsCount;
    cout << "SimpleLinearSearch - Operations: " << operationsCountByAlgorithm["SimpleLinearSearch"] << endl;

    operationsCount = 0;
    SearchAlgorithms::BinarySearch(megaArray, 0, 1000000, 1000001, operationsCount);
    operationsCountByAlgorithm["BinarySearch"] = operationsCount;
    cout << "BinarySearch - Operations: " << operationsCountByAlgorithm["BinarySearch"] << endl;

    operationsCount = 0;
    SearchAlgorithms::SentinelLinearSearch(megaArray, 999999, 1000000, operationsCount);
    operationsCountByAlgorithm["SentinelLinearSearch"] = operationsCount;
    cout << "SentinelLinearSearch - Operations: " << operationsCount << endl;
    return 0;
}