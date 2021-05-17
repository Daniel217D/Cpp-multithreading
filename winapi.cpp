#include<iostream>
#include<fstream>
#include<windows.h>

using std::ifstream;
using std::cout;

int task(int** matrix, int rows, int cols, int threads = 4);

unsigned __stdcall sum(void *);

int checkTaskNum(int num);

struct Data {
    int **matrix;
    int rows, cols, result = 1, fromCol, fromRow, toCol, toRow;
};

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

int task(int** matrix, int rows, int cols, int threads) {
    int res = 1;

    Data data[threads];
    HANDLE handles[threads];

    int length = (rows * cols) / threads;

    int fromRow = 0, fromCol = 0;
    for (size_t i = 0; i < threads; ++i) {
        data[i].matrix = matrix;
        data[i].rows = rows;
        data[i].cols = cols;
        data[i].fromCol = fromCol;
        data[i].fromRow = fromRow;

        //Расчёт позиции последнего элемента
        data[i].toCol = i == threads - 1 ? cols - 1 : fromCol + length - 1;
        data[i].toRow = i == threads - 1 ? rows - 1 : fromRow;
        if(data[i].toCol >= cols) {
            data[i].toRow = data[i].toCol / cols;
            data[i].toCol %= cols;
        }

        //Расчёт позиции первого элемента следующего потока
        fromCol = data[i].toCol + 1;
        fromRow = data[i].toRow;
        if(fromCol >= cols) {
            fromCol = 0;
            fromRow++;
        }

        handles[i] = (HANDLE) CreateThread(nullptr, 0, &sum, &data[i], 0, nullptr);
    }

    WaitForMultipleObjects(threads, handles, true, INFINITE);

    for (size_t i = 0; i < threads; ++i) {
//        cout << "from: " << data[i].fromRow << "," << data[i].fromCol << " - " << data[i].result << "\n";
        res *= data[i].result;
        CloseHandle(handles[i]);
    }

    return res;
}

unsigned __stdcall sum(void *info) {
    Data *data = (Data *) info;

    int row = data->fromRow, col = data->fromCol, temp;

    while(row != data->toRow || col != data->toCol) {
        temp = checkTaskNum(data->matrix[row][col]);
        if(temp != 0) {
            data->result *= temp;
        }

        col++;
        if(col >= data->cols) {
            col = 0;
            row++;
        }
    }

    temp = checkTaskNum(data->matrix[row][col]);
    if(temp != 0) {
        data->result *= temp;
    }

    return 0;
}

int checkTaskNum(int num) {
    int result = 0;

    if(num >= 10) {
        while(num >= 100) {
            num /= 10;
        }

        if(num % 2 != (num / 10) % 2) {
            result = num / 10;
        }
    }

    return result;
}