#include <iostream>
#include <omp.h>
#include <time.h>

using namespace std;

#define SIZE 500
typedef int BASE_TYPE;

double multiplyParallel(const int threadsAmount, BASE_TYPE** A, BASE_TYPE** B, BASE_TYPE** C) {
	const double start = omp_get_wtime();
	#pragma omp parallel for collapse(3) num_threads(threadsAmount)
	for (int i = 0; i < SIZE; i++)
		for (int u = 0; u < SIZE; u++)
			for(int t = 0; t < SIZE; t++)
				#pragma omp atomic
				C[i][u] += A[i][t] * B[t][u];

	return omp_get_wtime() - start;
}

double multiply(BASE_TYPE** A, BASE_TYPE** B, BASE_TYPE** C) {
	const double start = omp_get_wtime();
	for (int i = 0; i < SIZE; i++)
		for (int u = 0; u < SIZE; u++)
			for (int t = 0; t < SIZE; t++)
				C[i][u] += A[i][t] * B[t][u];

	return omp_get_wtime() - start;
}

void cleanMatrix(BASE_TYPE** mtx) {
	#pragma omp parallel for collapse(2)
	for (int i = 0; i < SIZE; i++)
		for (int u = 0; u < SIZE; u++)
			mtx[i][u] = 0;
}

int main() {
	srand(time(NULL));

	BASE_TYPE** A = new BASE_TYPE* [SIZE];
	BASE_TYPE** B = new BASE_TYPE* [SIZE];
	BASE_TYPE** C = new BASE_TYPE* [SIZE];

	for (int i = 0; i < SIZE; i++) {
		A[i] = new BASE_TYPE[SIZE];
		B[i] = new BASE_TYPE[SIZE];
		C[i] = new BASE_TYPE[SIZE];
		for (int u = 0; u < SIZE; u++) {
			A[i][u] = (BASE_TYPE) rand() % 100;
			B[i][u] = (BASE_TYPE) rand() % 100;
			C[i][u] = 0;
		}
	}

	for (int threads = 1; threads <= 8; threads *= 2) {
		const double calcTime = multiplyParallel(threads, A, B, C);
		cout << "Parallel (" << threads << " threads): " << calcTime << " sec." << endl;
		cleanMatrix(C);
	}

	double calcTime = multiply(A, B, C);
	cout << "Sequential: " << calcTime << " sec." << endl;

	#pragma omp parallel for
	for (int i = 0; i < SIZE; i++) {
		delete[] A[i];
		delete[] B[i];
		delete[] C[i];
	}

	delete[] A;
	delete[] B;
	delete[] C;

	return EXIT_SUCCESS;
}
