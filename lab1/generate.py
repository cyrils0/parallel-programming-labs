import random
import sys

n = int(sys.argv[1])

def generate_matrix(n):
    matrix = []
    for i in range(n):
        row = []
        for j in range(n):
            row.append(random.randint(-100, 100))
        matrix.append(row)
    return matrix

def save_matrix(matrix, filename):
    with open(filename, "w") as file:
        n = len(matrix)
        file.write(f"{n} {n}\n")
        for row in matrix:
            file.write(" ".join(map(str, row)) + "\n")

A = generate_matrix(n)
B = generate_matrix(n)
save_matrix(A, "input/matrixA.txt")
save_matrix(B, "input/matrixB.txt")