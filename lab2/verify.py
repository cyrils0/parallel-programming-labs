import numpy as np

def load_matrix(filename):
    with open(filename, "r") as file:
        rows, cols = map(int, file.readline().split())
        data = []
        for i in range(rows):
            row = list (map(float, file.readline().split()))
            data.append(row)
        return np.array(data)

A = load_matrix("input/matrixA.txt")
B = load_matrix("input/matrixB.txt")
C_python = A@B
C_cpp = load_matrix("output/result.txt")

if np.allclose(C_python, C_cpp):
    print("результат корректен")
else:
    print("результат не корректен")

print("Результат Python:")
print(C_python)

print("Результат C++:")
print(C_cpp)

