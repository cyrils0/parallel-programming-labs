#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <string>
#include <chrono>
#include <cuda_runtime.h>

using namespace std;

struct Matrix
{
    int rows;
    int cols;
    vector<double> data;
};

Matrix LoadMatrix(const string& filename)
{
    ifstream file(filename);

    if (!file.is_open())
    {
        throw runtime_error("Cannot open file: " + filename);
    }

    Matrix matrix{};
    file >> matrix.rows >> matrix.cols;

    if (!file || matrix.rows <= 0 || matrix.cols <= 0)
    {
        throw runtime_error("Invalid matrix dimensions");
    }

    matrix.data.resize(
        static_cast<size_t>(matrix.rows) * matrix.cols
    );

    for (double& value : matrix.data)
    {
        if (!(file >> value))
        {
            throw runtime_error("Invalid matrix data");
        }
    }

    return matrix;
}

void SaveMatrix(const Matrix& matrix, const string& filename)
{
    ofstream file(filename);

    if (!file.is_open())
    {
        throw runtime_error("Cannot create file: " + filename);
    }

    file << matrix.rows << " " << matrix.cols << "\n";

    for (int i = 0; i < matrix.rows; i++)
    {
        for (int j = 0; j < matrix.cols; j++)
        {
            file << matrix.data[
                static_cast<size_t>(i) * matrix.cols + j
            ] << " ";
        }

        file << "\n";
    }
}

void CheckCuda(cudaError_t error, const string& operation)
{
    if (error != cudaSuccess)
    {
        throw runtime_error(
            operation + ": " + cudaGetErrorString(error)
        );
    }
}

__global__ void MultiplyMatrices(
    const double* A,
    const double* B,
    double* C,
    int rowsA,
    int colsA,
    int colsB)
{
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < rowsA && col < colsB)
    {
        double sum = 0.0;

        for (int k = 0; k < colsA; k++)
        {
            sum += A[row * colsA + k] *
                   B[k * colsB + col];
        }

        C[row * colsB + col] = sum;
    }
}

int main(int argc, char* argv[])
{
    try
    {
        int blockDimension = 16;

        if (argc > 2)
        {
            throw runtime_error("Usage: lab4.exe [block_size]");
        }

        if (argc == 2)
        {
            blockDimension = stoi(argv[1]);
        }

        if (blockDimension < 1 || blockDimension > 32)
        {
            throw runtime_error("Block size must be between 1 and 32");
        }

        Matrix A = LoadMatrix("lab4/input/matrixA.txt");
        Matrix B = LoadMatrix("lab4/input/matrixB.txt");

        if (A.cols != B.rows)
        {
            throw runtime_error("Incompatible matrix dimensions");
        }

        cout << "Matrix size: "
             << A.rows << " x " << A.cols << "\n";

        Matrix C{};
        C.rows = A.rows;
        C.cols = B.cols;
        C.data.resize(
            static_cast<size_t>(C.rows) * C.cols
        );

        double* deviceA = nullptr;
        double* deviceB = nullptr;
        double* deviceC = nullptr;

        size_t bytesA = A.data.size() * sizeof(double);
        size_t bytesB = B.data.size() * sizeof(double);
        size_t bytesC = C.data.size() * sizeof(double);

        CheckCuda(
            cudaMalloc(&deviceA, bytesA),
            "cudaMalloc A"
        );

        CheckCuda(
            cudaMalloc(&deviceB, bytesB),
            "cudaMalloc B"
        );

        CheckCuda(
            cudaMalloc(&deviceC, bytesC),
            "cudaMalloc C"
        );

        CheckCuda(
            cudaMemcpy(deviceA, A.data.data(), bytesA,
                       cudaMemcpyHostToDevice),
            "Copy A to GPU"
        );

        CheckCuda(
            cudaMemcpy(deviceB, B.data.data(), bytesB,
                       cudaMemcpyHostToDevice),
            "Copy B to GPU"
        );

        dim3 blockSize(blockDimension, blockDimension);
        dim3 gridSize(
            (B.cols + blockSize.x - 1) / blockSize.x,
            (A.rows + blockSize.y - 1) / blockSize.y
        );

        
        constexpr int repetitions = 5;

        cudaEvent_t start, stop;

        CheckCuda(cudaEventCreate(&start), "Create start event");
        CheckCuda(cudaEventCreate(&stop), "Create stop event");

        MultiplyMatrices<<<gridSize, blockSize>>>(
            deviceA, deviceB, deviceC,
            A.rows, A.cols, B.cols
        );

        CheckCuda(cudaGetLastError(), "Warm-up kernel launch");
        CheckCuda(cudaDeviceSynchronize(), "Warm-up synchronization");

        CheckCuda(cudaEventRecord(start), "Record start event");

        for (int i = 0; i < repetitions; i++)
        {
            MultiplyMatrices<<<gridSize, blockSize>>>(
                deviceA, deviceB, deviceC,
                A.rows, A.cols, B.cols
            );
        }

        CheckCuda(cudaGetLastError(), "Launch kernel");

        CheckCuda(cudaEventRecord(stop), "Record stop event");
        CheckCuda(cudaEventSynchronize(stop), "Synchronize");

        float totalKernelTime = 0.0f;

        CheckCuda(
            cudaEventElapsedTime(&totalKernelTime, start, stop),
            "Measure kernel time"
        );

        float kernelTime = totalKernelTime / repetitions;



        CheckCuda(
            cudaMemcpy(C.data.data(), deviceC, bytesC,
                       cudaMemcpyDeviceToHost),
            "Copy result from GPU"
        );

        SaveMatrix(C, "lab4/output/result.txt");

        cout << "Block size: "
             << blockSize.x << " x " << blockSize.y << "\n";

        cout << "Grid size: "
             << gridSize.x << " x " << gridSize.y << "\n";

        cout << "Average kernel time ("
            << repetitions << " runs): "
            << kernelTime << " ms\n";
        cudaEventDestroy(start);
        cudaEventDestroy(stop);

        cudaFree(deviceA);
        cudaFree(deviceB);
        cudaFree(deviceC);

        cout << "Result saved successfully\n";
    }
    catch (const exception& e)
    {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
