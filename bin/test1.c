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

#define M 800
#define N 800
#define K 800
// #define M 100
// #define N 100
// #define K 100

// 初始化矩阵
void initialize_matrix(float matrix[M][N], int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = (float)rand() / RAND_MAX;  // 随机初始化元素
        }
    }
}

// 矩阵乘法
void matrix_multiply(float A[M][K], float B[K][N], float C[M][N]) {
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
    float A[M][K];
    float B[K][N];
    float C[M][N];

    // 初始化矩阵 A 和 B
    initialize_matrix(A, M, K);
    initialize_matrix(B, K, N);

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

    return 0;
}
