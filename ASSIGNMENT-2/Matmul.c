/*
 * Matrix Multiplication - comparing four versions:
 *   1. Naive
 *   2. Loop interchange
 *   3. Loop tiling
 *   4. Loop unrolling
 *
 * Build: gcc -O2 matmul.c -o matmul
 * Run:   ./matmul      (Windows: .\matmul.exe)
 *
 * Note: N must be a multiple of T and of 4.
 */

#include <stdio.h>
#include <time.h>

#define N 512     // matrix size (N x N)
#define T 64      // tile size for the tiled version

int A[N][N], B[N][N], C[N][N], ref[N][N];


// Fill A and B with some starting values.
void init(void) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i][j] = (i + j) % 10;
            B[i][j] = (i - j) % 10;
        }
    }
}

// Set C back to zero before a version that adds into it.
void clear(void) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            C[i][j] = 0;
        }
    }
}


// 1) Naive version
void naive(void) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int sum = 0;
            for (int k = 0; k < N; k++) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
}

// 2) Loop interchange: order changed to i, k, j
void interchange(void) {
    clear();
    for (int i = 0; i < N; i++) {
        for (int k = 0; k < N; k++) {
            int r = A[i][k];
            for (int j = 0; j < N; j++) {
                C[i][j] += r * B[k][j];
            }
        }
    }
}

// 3) Loop tiling: work on small T x T blocks
void tiled(void) {
    clear();
    for (int ii = 0; ii < N; ii += T) {
        for (int kk = 0; kk < N; kk += T) {
            for (int jj = 0; jj < N; jj += T) {
                for (int i = ii; i < ii + T; i++) {
                    for (int k = kk; k < kk + T; k++) {
                        int r = A[i][k];
                        for (int j = jj; j < jj + T; j++) {
                            C[i][j] += r * B[k][j];
                        }
                    }
                }
            }
        }
    }
}

// 4) Loop unrolling: do 4 columns of the inner loop at a time
void unrolled(void) {
    clear();
    for (int i = 0; i < N; i++) {
        for (int k = 0; k < N; k++) {
            int r = A[i][k];
            for (int j = 0; j < N; j += 4) {
                C[i][j]     += r * B[k][j];
                C[i][j + 1] += r * B[k][j + 1];
                C[i][j + 2] += r * B[k][j + 2];
                C[i][j + 3] += r * B[k][j + 3];
            }
        }
    }
}


// Run a version and return how long it took, in seconds.
double timeit(void (*func)(void)) {
    clock_t start = clock();
    func();
    clock_t end = clock();
    return (double)(end - start) / CLOCKS_PER_SEC;
}

// Check that C matches the naive result stored in ref.
int correct(void) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (C[i][j] != ref[i][j]) {
                return 0;
            }
        }
    }
    return 1;
}


int main(void) {
    init();

    // Run naive first and keep its result as the correct answer.
    double t0 = timeit(naive);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            ref[i][j] = C[i][j];
        }
    }

    double t1 = timeit(interchange);
    int c1 = correct();

    double t2 = timeit(tiled);
    int c2 = correct();

    double t3 = timeit(unrolled);
    int c3 = correct();

    printf("Matrix size %d x %d, tile size %d\n\n", N, N, T);
    printf("Naive         %.4f s   1.00x\n", t0);
    printf("Interchange   %.4f s   %.2fx   %s\n", t1, t0 / t1, c1 ? "ok" : "WRONG");
    printf("Tiling        %.4f s   %.2fx   %s\n", t2, t0 / t2, c2 ? "ok" : "WRONG");
    printf("Unrolling     %.4f s   %.2fx   %s\n", t3, t0 / t3, c3 ? "ok" : "WRONG");

    return 0;
}