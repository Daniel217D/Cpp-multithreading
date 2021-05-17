#include<iostream>
#include<fstream>
#include <thread>
#include<windows.h>

using std::thread;
using std::ifstream;
using std::cout;

struct Data {
    int **matrix;
    int rows, cols, fromCol, fromRow, toCol, toRow;
};

int task(int **matrix, int rows, int cols, int threads = 4);

unsigned __stdcall sum(Data *data, int volatile &result);

int checkTaskNum(int num);

int main() {
    ifstream fin("input.txt");
    if (!fin.is_open()) {
        cout << "Ошибка открытия файла!";
    } else {
        int rows, cols;

        fin >> rows;
        fin >> cols;

        int **matrix = new int *[rows];

        for (size_t i = 0; i < rows; ++i) {
            matrix[i] = new int[cols];
            for (size_t j = 0; j < cols; ++j) {
                fin >> matrix[i][j];
            }
        }

        cout << task(matrix, rows, cols);
    }


    return 0;
}

int task(int **matrix, int rows, int cols, int numOfThreads) {
    int volatile result = 1;

    Data data[numOfThreads];
    thread threads[numOfThreads];

    int length = (rows * cols) / numOfThreads;

    int fromRow = 0, fromCol = 0;
    for (size_t i = 0; i < numOfThreads; ++i) {
        data[i].matrix = matrix;
        data[i].rows = rows;
        data[i].cols = cols;
        data[i].fromCol = fromCol;
        data[i].fromRow = fromRow;

        //Расчёт позиции последнего элемента
        data[i].toCol = i == numOfThreads - 1 ? cols - 1 : fromCol + length - 1;
        data[i].toRow = i == numOfThreads - 1 ? rows - 1 : fromRow;
        if (data[i].toCol >= cols) {
            data[i].toRow = data[i].toCol / cols;
            data[i].toCol %= cols;
        }

        //Расчёт позиции первого элемента следующего потока
        fromCol = data[i].toCol + 1;
        fromRow = data[i].toRow;
        if (fromCol >= cols) {
            fromCol = 0;
            fromRow++;
        }

        threads[i] = thread(&sum, &data[i], std::ref(result));
    }

    for (int i = 0; i < numOfThreads; i++) {
        threads[i].join();
    }

    return result;
}

unsigned __stdcall sum(Data *data, int volatile &result) {
    int row = data->fromRow, col = data->fromCol, temp, localResult = 1;
    while (row != data->toRow || col != data->toCol) {
        temp = checkTaskNum(data->matrix[row][col]);
        if (temp != 0) {
            localResult *= temp;
        }

        col++;
        if (col >= data->cols) {
            col = 0;
            row++;
        }
    }

    temp = checkTaskNum(data->matrix[row][col]);
    if (temp != 0) {
        localResult *= temp;
    }

    _InterlockedExchange(&result, result * localResult);

    return 0;
}

int checkTaskNum(int num) {
    int result = 0;

    if (num >= 10) {
        while (num >= 100) {
            num /= 10;
        }

        if (num % 2 != (num / 10) % 2) {
            result = num / 10;
        }
    }

    return result;
}