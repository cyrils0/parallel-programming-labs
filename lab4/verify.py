import numpy as np


def load_matrix(filename):
    with open(filename, "r") as file:
        rows, cols = map(int, file.readline().split())
        data = []

        for i in range(rows):
            row = list(map(float, file.readline().split()))
            data.append(row)

        return np.array(data)


A = load_matrix("input/matrixA.txt")
B = load_matrix("input/matrixB.txt")
C_gpu = load_matrix("output/result.txt")

C_python = A @ B

if np.allclose(C_python, C_gpu):
    print("CUDA result is correct!")
else:
    print("CUDA result is incorrect!")
    print("Maximum error:", np.max(np.abs(C_python - C_gpu)))