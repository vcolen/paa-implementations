#include <iostream>
using namespace std;

class ArraySearcher
{

public:
    static bool SimpleLinearSearch(int *array, int sizeOfArray, int x)
    {
        for (int i = 0; i < sizeOfArray; i++)
        {
            if (x == array[i])
            {
                return true;
            }
        }
        return false;
    }

    static bool SentinelLinearSearch(int *array, int lastIndex, int x)
    {
        int i = 0;
        array[lastIndex] = x;
        while (array[i] != x)
            i++;
        return i != lastIndex;
    }

    static bool BinarySearch(int *array, int left, int right, int x)
    {
        while (left <= right)
        {
            int mid = left + (right - left) / 2;
            // verificando se x está no meio
            if (array[mid] == x)
                return true;
            // se x for maior, ignore a metade da esquerda
            if (array[mid] < x)
                left = mid + 1;
            // se x for menor, ignore a metade dadireita
            else
                right = mid - 1;
        }
        return false;
    }
};

int main()
{
    int test[11] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    cout << ArraySearcher::SimpleLinearSearch(test, 11, 11) << "\n";
    cout << ArraySearcher::BinarySearch(test, 0, 11, 11) << "\n";
    cout << ArraySearcher::SentinelLinearSearch(test, 10, 11);
    return 0;
}