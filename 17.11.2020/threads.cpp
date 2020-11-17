#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <iostream>
#include <pthread.h>
#include <vector>
#include <cmath>

using namespace std;

// Двоичный семафор.
pthread_mutex_t mutex1;
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
void* func(void* param) {
    // Размерность подматрицы m x m.
    int m = *(int*)param;
    // Квадратная подматрица размерностью m x m (m на отрезке [1; n]).
    vector<vector<int>> subMatrix;
    if (m == n) {
        subMatrix = matrixA;
    } else {
        subMatrix = getSubMatrix(matrixA, m);
    }

    int rang = computeRank(subMatrix);

    // Протокол входа в КС: закрыть двоичный семафор.
    pthread_mutex_lock(&mutex1);

    maxRang = rang > maxRang ? rang : maxRang;

    printf("Поток посчитал ранг минора %d x %d: %d\n", m, m, rang);

    // Протокол выхода из КС: открыть двоичный семафор.
    pthread_mutex_unlock(&mutex1);
}

int main (int argc, char *argv[]) {
    int n = stoi(argv[1]);
    int numOfThreads = stoi(argv[2]);
    matrixA;

    // Инициализация двоичного семафора
    pthread_mutex_init(&mutex1, NULL);

    /*srand(time(NULL));*/

    for (int i = 0; i < n; ++i) {
        cout << "Enter matrix numbers for line #" << i + 1 << endl;
        vector<int> tempVector;
        matrixA.push_back(tempVector);

        for (int j = 0; j < n; ++j) {
            string numToAdd;
            cin >> numToAdd;
            matrixA[i].push_back(stoi(numToAdd));
            /*matrixA[i].push_back(rand() % 100);
            cout << matrixA[i][j] << endl;*/
        }

        printf("\n");
    }

    // Идентификаторы для дочерних потоков.
    pthread_t threads[numOfThreads - 1];

    int num[n];

    for (int i = 0; i < n; i++) {
        // Номера размерности подматрицы для потоков.
        num[i] = i + 1;
    }

    if (numOfThreads == 1) {
        // Если поток один, то через главный находим ранг матрицы.
        int counter = 0;

        for (int i = 0; i < n; ++i) {
            func((void *)(num + counter));
            ++counter;
        }
    } else {
        if (numOfThreads < n) {
            // Нахождение ранга матрицы размерностью 1 x 1.
            func((void *)num);

            int counterCreate = 0;
            int counterJoin = 0;

            do {
                for (int i = 0; i < numOfThreads - 1; ++i) {
                    if (counterCreate == n - 1) {
                        break;
                    }

                    pthread_create(&threads[i], NULL, func, (void*)(num + counterCreate + 1));
                    ++counterCreate;

                }

                // Нахождение ранга матрицы размерностей кроме 1 x 1.
                for (int i = 0; i < numOfThreads - 1; ++i) {
                    if (counterJoin == n - 1) {
                        break;
                    }

                    pthread_join(threads[i], NULL);
                    ++counterJoin;
                }
            } while (counterCreate < n - 1);
        } else {
            for (int i = 0; i < n - 1; ++i) {
                pthread_create(&threads[i], NULL, func, (void*)(num + i + 1));
            }
            // Нахождение ранга матрицы размерностью 1 x 1.
            func((void *)num);

            // Нахождение ранга матрицы остальных размерностей.
            for (int i = 0; i < n - 1; ++i) {
                pthread_join(threads[i], NULL);
            }
        }
    }

    cout << "Matrix A rank is " << maxRang << endl;
}
