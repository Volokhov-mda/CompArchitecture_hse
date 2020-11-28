#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <vector>
#include <cmath>
#include <iostream>
#include <omp.h>

// Launch prog command (tested on unix system)
// clang++ -Xpreprocessor -fopenmp -std=c++11 -I/usr/local/include -L/usr/local/lib -lomp main.cpp -o main && ./main <first_arg> <second_arg>

using namespace std;

// Матрица A.
vector<vector<int>> matrixA;
// Размерность матрицы A.
int n;
// Максимальный ранг подматрицы.
int maxRang = 0;

const double EPS = 1E-9;

int computeRank(vector<vector<int>> A) {
    int n = A.size();
    int m = A[0].size();

    int rank = 0;
    vector<bool> row_selected(n, false);
    for (int i = 0; i < m; ++i) {
        int j;
        for (j = 0; j < n; ++j) {
            if (!row_selected[j] && abs(A[j][i]) > EPS)
                break;
        }

        if (j != n) {
            ++rank;
            row_selected[j] = true;

            for (int p = i + 1; p < m; ++p)
                A[j][p] /= A[j][i];

            for (int k = 0; k < n; ++k) {
                if (k != j && abs(A[k][i]) > EPS) {
                    for (int p = i + 1; p < m; ++p)
                        A[k][p] -= A[j][p] * A[k][i];
                }
            }
        }
    }
    return rank;
}

// Возвращает подматрицу размером m x m.
vector<vector<int>> getSubMatrix(vector<vector<int>> matrix, int m) {
    vector<vector<int>> subMatrix;

    for (int i = 0; i < m; ++i) {
        vector<int> tempVector;
        subMatrix.push_back(tempVector);

        for (int j = 0; j < m; ++j) {
            subMatrix[i].push_back(matrix[i][j]);
        }
    }

    return subMatrix;
}

// Стартовая функция для дочерних потоков.
void func(int matrixSize, int numOfThreads ) {
    #pragma omp parallel num_threads(numOfThreads)
    {
        // Квадратная подматрица размерностью m x m (m на отрезке [1; n]).
        vector<vector<int>> subMatrix;
        int rang;

        #pragma omp for
        for (int i = 1; i <= matrixSize; i++) {
            if (i == n) {
                subMatrix = matrixA;
            } else {
                subMatrix = getSubMatrix(matrixA, i);
            }

            rang = computeRank(subMatrix);

            #pragma omp critical
            {
                maxRang = rang > maxRang ? rang : maxRang;

                printf("Thread #%d calculated rank of minor %d x %d: %d\n", omp_get_thread_num() + 1, i, i, rang);
            }
        }
    }
}

int THREAD_NUM;

int main(int argc, char* argv[]) {
    const int matrixSize = stoi(argv[1]);
    const int THREAD_NUM = stoi(argv[2]);

    srand(time(NULL));

    for (int i = 0; i < matrixSize; ++i) {
        cout << "Enter matrix numbers for line #" << i + 1 << endl;
        vector<int> tempVector;
        matrixA.push_back(tempVector);

        for (int j = 0; j < matrixSize; ++j) {
//            string numToAdd;
//            cin >> numToAdd;
//            matrixA[i].push_back(stoi(numToAdd));
            matrixA[i].push_back(rand() % 100);
            cout << matrixA[i][j] << endl;
        }

        printf("\n");
    }

    func(matrixSize, THREAD_NUM);

    cout << "Matrix A rank is " << maxRang << endl;
}
