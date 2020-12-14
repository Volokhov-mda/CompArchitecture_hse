#include <stdio.h>
#include <unistd.h>
#include <string>
#include <cmath>
#include <pthread.h>
#include <iostream>

using namespace std;

// Двоичный семафор для вывода информации о съеденных кусках.
pthread_mutex_t mutex1;

int currPiecesNum;
int BOWL_CAPACITY;

// Стартовая функция для дочерних потоков.
void* func(void* param) {
    int* threadNum = (int*)param;
    // Протокол входа в КС: закрыть двоичный семафор.
    pthread_mutex_lock(&mutex1);
    if (currPiecesNum != 0) {
        usleep(500000);

        printf("Каннибал %d съел кусок. Осталось %d кусков\n", threadNum, --currPiecesNum);

        usleep(500000);
    } else {
        printf("\nКаннибалу %d не досталось куска, он будит повара\n", threadNum);

        usleep(2000000);

        currPiecesNum = BOWL_CAPACITY;

        printf("Повар разбужен. Теперь в горшке снова %d кусков\n\n", currPiecesNum);
    }

    // Протокол выхода из КС: открыть двоичный семафор.
    pthread_mutex_unlock(&mutex1);
}

bool IsNumber(char* number)
{
    int i = 0;

    if (number[0] == '-') {
        i = 1;
    }

    for (i; i < strlen(number); ++i) {
        if (!isdigit(number[i])) {
            cout << number[i] << endl;
            return false;
        }
    }

    return true;
}

int main (int argc, char *argv[]) {
    if (argc == 3) {
        if ((!IsNumber(argv[1])) ||
            (!IsNumber(argv[2])))
        {
            cout << "Неверные входные параметры: параметры должны быть числами. Завершение программы" << endl;
            return 0;
        }
    } else if (argc == 4) {
        if ((!IsNumber(argv[1])) ||
            (!IsNumber(argv[2])) ||
            (!IsNumber(argv[3])))
        {
            cout << "Неверные входные параметры: параметры должны быть числами. Завершение программы" << endl;
            return 0;
        }
    } else {
        cout << "Неверное число входных параметров: их должно быть 2 или 3. Завершение программы" << endl;
        return 0;
    }

    // Количество потоков (= количеству каннибалов).
    int threadsNum = stoi(argv[1]);
    // Количество задач (= количеству кусков).
    int tasksNum = currPiecesNum = BOWL_CAPACITY = stoi(argv[2]);
    // Количетсво итераций (= раз, когда горшок с мясом будет полным).
    int iterationsNum = (argc == 3) ? 2 : stoi(argv[3]);

    if (threadsNum <= 0 || tasksNum <= 0 || iterationsNum <= 0) {
        cout << "Неверные входные параметры: параметры должны быть положительными числами. Завершение программы" << endl;
        exit(0);
    }

    printf("%d каннибалов и %d кусков помещается в горшке, горшок заполняется %d раз\n\n", threadsNum, tasksNum, iterationsNum);

    if (threadsNum == 1) {
        for (int iterationNum = 0; iterationNum < iterationsNum; ++iterationNum) {
            for (int i = 0; i < tasksNum + 1; i++) {
                // В последней итерации не надо наполнять миску.
                if (iterationNum == iterationsNum - 1 && i == tasksNum) {
                    break;
                }

                cout << "i: " << i << endl;
                func((void *)(1));
            }
        }
    } else {
        // Инициализация двоичного семафора
        pthread_mutex_init(&mutex1, NULL);
        // Идентификаторы для дочерних потоков.
        pthread_t threads[threadsNum];

        int numOfThreadsCreated = 0;
        int numOfTasksDone = 0;

        do {
            for (int i = 0; i < threadsNum; ++i) {
                pthread_create(&threads[i], NULL, func, (void*)(i + 1));

                ++numOfThreadsCreated;

                if (numOfThreadsCreated == iterationsNum * tasksNum + iterationsNum - 1) {
                    break;
                }
            }

            for (int i = 0; i < threadsNum; ++i) {
                pthread_join(threads[i], NULL);

                ++numOfTasksDone;

                if (numOfTasksDone == iterationsNum * tasksNum + iterationsNum - 1) {
                    break;
                }
            }
        } while (numOfTasksDone != iterationsNum * tasksNum + iterationsNum - 1);
    }

    printf("\nМиска больше наполняться не будет, повар отказывается\n");
}
