#include <iostream>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <chrono>
#include <string>
#include <mpi.h>

using namespace std;

class Matrix
{
private:
    int _rows;
    int _cols;
    vector<double> _data;

public:
    Matrix() : _rows(0), _cols(0) {}

    Matrix(int rows, int cols)
    {
        if (rows <= 0 || cols <= 0)
        {
            throw invalid_argument("Неверный размер матрицы");
        }

        _rows = rows;
        _cols = cols;
        _data.assign(rows * cols, 0.0);
    }

    int rows() const
    {
        return _rows;
    }

    int cols() const
    {
        return _cols;
    }

    double& operator()(int row, int col)
    {
        return _data[row * _cols + col];
    }

    const double& operator()(int row, int col) const
    {
        return _data[row * _cols + col];
    }

    void LoadFromFile(const string& filename)
    {
        ifstream file(filename);

        if (!file.is_open())
        {
            throw runtime_error("Не удалось открыть файл: " + filename);
        }

        file >> _rows >> _cols;

        if (!_rows || !_cols || _rows < 0 || _cols < 0)
        {
            throw runtime_error("Неверный размер матрицы");
        }

        _data.assign(_rows * _cols, 0.0);

        for (int i = 0; i < _rows; i++)
        {
            for (int j = 0; j < _cols; j++)
            {
                if (!(file >> _data[i * _cols + j]))
                {
                    throw runtime_error("Ошибка чтения элементов матрицы");
                }
            }
        }
    }

    void SaveToFile(const string& filename) const
    {
        ofstream file(filename);

        if (!file.is_open())
        {
            throw runtime_error("Не удалось открыть файл: " + filename);
        }

        file << _rows << " " << _cols << "\n";

        for (int i = 0; i < _rows; i++)
        {
            for (int j = 0; j < _cols; j++)
            {
                file << (*this)(i, j) << " ";
            }

            file << "\n";
        }
    }

    double* data()
    {
        return _data.data();
    }

    const double* data() const
    {
        return _data.data();
    }
};

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int rank = 0;
    int processCount = 1;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &processCount);

    try
    {
        Matrix A;
        Matrix B;
        Matrix C;

        int n = 0;
        int m = 0;
        int bRows = 0;
        int bCols = 0;

        if (rank == 0)
        {
            A.LoadFromFile("matrixA.txt");
            B.LoadFromFile("matrixB.txt");

            n = A.rows();
            m = A.cols();
            bRows = B.rows();
            bCols = B.cols();

            if (m != bRows)
            {
                throw invalid_argument(
                    "Количество столбцов A должно совпадать с количеством строк B");
            }

            C = Matrix(n, bCols);
        }

        MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&bRows, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&bCols, 1, MPI_INT, 0, MPI_COMM_WORLD);

        Matrix localA(n > 0 ? (n + processCount - 1) / processCount : 1, m > 0 ? m : 1);

        int baseRows = n / processCount;
        int remainder = n % processCount;
        int localRows = baseRows + (rank < remainder ? 1 : 0);

        localA = Matrix(localRows > 0 ? localRows : 1, m);

        vector<int> sendCounts(processCount);
        vector<int> sendDisplacements(processCount);
        vector<int> receiveCounts(processCount);
        vector<int> receiveDisplacements(processCount);

        int offset = 0;

        for (int i = 0; i < processCount; i++)
        {
            int rowsForProcess = baseRows + (i < remainder ? 1 : 0);

            sendCounts[i] = rowsForProcess * m;
            sendDisplacements[i] = offset;
            offset += sendCounts[i];

            receiveCounts[i] = rowsForProcess * bCols;
            receiveDisplacements[i] = (sendDisplacements[i] / m) * bCols;
        }

        vector<double> flatB;

        if (rank == 0)
        {
            flatB.assign(B.data(), B.data() + bRows * bCols);
        }
        else
        {
            flatB.resize(static_cast<size_t>(bRows) * bCols);
        }

        MPI_Bcast(
            flatB.data(),
            bRows * bCols,
            MPI_DOUBLE,
            0,
            MPI_COMM_WORLD);

        double startTime = 0.0;

        if (rank == 0)
        {
            startTime = MPI_Wtime();
        }

        MPI_Scatterv(
            rank == 0 ? A.data() : nullptr,
            sendCounts.data(),
            sendDisplacements.data(),
            MPI_DOUBLE,
            localA.data(),
            localRows * m,
            MPI_DOUBLE,
            0,
            MPI_COMM_WORLD);

        Matrix localC(localRows, bCols);

        for (int i = 0; i < localRows; i++)
        {
            for (int j = 0; j < bCols; j++)
            {
                for (int k = 0; k < m; k++)
                {
                    localC(i, j) += localA(i, k) * flatB[k * bCols + j];
                }
            }
        }

        MPI_Gatherv(
            localC.data(),
            localRows * bCols,
            MPI_DOUBLE,
            rank == 0 ? C.data() : nullptr,
            receiveCounts.data(),
            receiveDisplacements.data(),
            MPI_DOUBLE,
            0,
            MPI_COMM_WORLD);

        if (rank == 0)
        {
            double endTime = MPI_Wtime();

            cout << "Размер матриц: " << n << " x " << m << "\n";
            cout << "Количество MPI-процессов: " << processCount << "\n";
            cout << "Время выполнения: "
                 << (endTime - startTime) * 1000.0 << " мс\n";

            C.SaveToFile("output/result.txt");
            cout << "Результат сохранён в output/result.txt\n";
        }
    }
    catch (const exception& e)
    {
        cerr << "Процесс " << rank << ": ошибка: " << e.what() << endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    MPI_Finalize();
    return 0;
}
