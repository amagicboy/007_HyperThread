#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

// 函数用于获取当前时间戳（微秒级）
uint64_t get_timestamp() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000 + (uint64_t)ts.tv_nsec / 1000;
}

#define M 2000
#define N 2000
#define K 2000

// 初始化矩阵
float** initialize_matrix(int rows, int cols) {
    float** matrix = (float**)malloc(rows * sizeof(float*));
    for (int i = 0; i < rows; i++) {
        matrix[i] = (float*)malloc(cols * sizeof(float));
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = (float)rand() / RAND_MAX;  // 随机初始化元素
        }
    }
    return matrix;
}

// 矩阵乘法
void matrix_multiply(float** A, float** B, float** C) {
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            C[i][j] = 0.0;
            for (int k = 0; k < K; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main() {
    float** A = initialize_matrix(M, K);
    float** B = initialize_matrix(K, N);
    float** C = (float**)malloc(M * sizeof(float*));
    for (int i = 0; i < M; i++) {
        C[i] = (float*)malloc(N * sizeof(float));
    }

    // 记录开始时间戳
    uint64_t start_time = get_timestamp();

    // 执行矩阵乘法
    matrix_multiply(A, B, C);

    // 记录结束时间戳
    uint64_t end_time = get_timestamp();

    // 打印性能数据
    printf("Time taken: %lu microseconds\n", end_time - start_time);

    // 打印结果矩阵的某些元素
    printf("Resulting Matrix C[0][0]: %0.2f\n", C[0][0]);
    printf("Resulting Matrix C[1][1]: %0.2f\n", C[1][1]);
    printf("Resulting Matrix C[M-1][N-1]: %0.2f\n", C[M - 1][N - 1]);

    // 释放动态分配的内存
    for (int i = 0; i < M; i++) {
        free(A[i]);
        free(B[i]);
        free(C[i]);
    }
    free(A);
    free(B);
    free(C);

    return 0;
}
