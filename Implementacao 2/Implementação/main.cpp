#include <iostream>
#include <map>
#include <fstream>
#include <cstdlib>

using namespace std;

class SearchAlgorithms
{

public:
    static bool SimpleLinearSearch(int *array, int sizeOfArray, int x, std::map<std::string, int> &dict)
    {
        dict["SimpleLinearSearch"] += 2; // int i; i = 0;
        for (int i = 0; i < sizeOfArray; i++)
        {
            if (x == array[i])
            {
                dict["SimpleLinearSearch"]++; // return
                return true;
            }
            dict["SimpleLinearSearch"]+=2; // x == array[i]; i < sizeOfArray
        }
        dict["SimpleLinearSearch"] += 2; // i < sizeOfArray; return false
        return false; 
    }

    static bool SentinelLinearSearch(int *array, int lastIndex, int x, std::map<std::string, int> &dict)
    {
        int i = 0;
        array[lastIndex] = x;
        while (array[i] != x)
            i++;
        return i != lastIndex;
    }

    static bool BinarySearch(int *array, int left, int right, int x, std::map<std::string, int> &dict)
    {
        while (left <= right)
        {
            int mid = left + (right - left) / 2;
            if (array[mid] == x)
                return true;
            if (array[mid] < x)
                left = mid + 1;
            else
                right = mid - 1;
        }
        return false;
    }
};

void populateMegaArray(int *megaArray) {
    for(int i = 0; i < 1000000; i++) {
        megaArray[i] = i;
    }
}
void ExportToCsv(int operationCount)
{
    std::ofstream file("operations_count.csv");
    file << "Algorithm,Operations\n";
    file << "SimpleLinearSearch," << operationCount << "\n";
    file << "BinarySearch," << operationCount << "\n";
    file << "SentinelLinearSearch," << operationCount << "\n";
    file.close();
}

void PlotCharts()
{
    system("python3 plot_operations.py"); // On Linux or MacOS
    // system("python plot_operations.py");  // On Windows
}

int main()
{
    int megaArray[1000000];
    populateMegaArray(megaArray);
    std::map<std::string, int> operationCountByAlgorithm;
    operationCountByAlgorithm["SimpleLinearSearch"] = 0;
    SearchAlgorithms::SimpleLinearSearch(megaArray, 1000000, 1000001, operationCountByAlgorithm);
    cout << "SimpleLinearSearch - Operations: " << operationCountByAlgorithm["SimpleLinearSearch"] << endl;
    //operationCountByAlgorithm["BinarySearch"] = 0;
    // int result2 = SearchAlgorithms::BinarySearch(test, 0, 10, 11, operationCount);
    // cout << "BinarySearch - Result: " << result2 << ", Operations: " << operationCount << endl;
    // operationCountByAlgorithm["SentinelLinearSearch"] = 0;
    // int result3 = SearchAlgorithms::SentinelLinearSearch(test, 10, 11, operationCount);
    // cout << "SentinelLinearSearch - Result: " << result3 << ", Operations: " << operationCount << endl;
    return 0;
}