#include <iostream>
#include <omp.h>
#include <random>
#include <math.h>

#define ARR_LENGTH 10
#define THREADS 12
#define PARALLEL true

double calcSum(double* arr, const int length) {
	double res = 0.0;
  #pragma omp parallel for reduction(+: res) if (PARALLEL)
	for (int i = 0; i < length; i++) res += arr[i];
	return res;
}

void fillArrRand(double* arr, const int length) {
  #pragma omp parallel for if (PARALLEL)
	for (int i = 0; i < length; i++)
		arr[i] = (double)rand() / (double)RAND_MAX;
}

void showArr(double* arr, const int length) {
	for (int i = 0; i < length; i++)
		std::cout << arr[i] << std::endl;
}

int main() {
  omp_set_num_threads(THREADS);

  const int layers[ARR_LENGTH + 1] = { ARR_LENGTH, 10, 30, 30, 40, 60, 60, 40, 30, 30, 1 };
  double* arr = new double[ARR_LENGTH];
	fillArrRand(arr, ARR_LENGTH);
	double* arrIterable = arr;

  std::cout << "Input array:\n";
	showArr(arr, ARR_LENGTH);

  std::random_device rd;  //Will be used to obtain a seed for the random number engine
  std::mt19937 generator(rd()); //Standard mersenne_twister_engine seeded with rd()
  std::uniform_real_distribution<double> distribution(0.0, 1.0);

  const double start = omp_get_wtime();
  for (int i = 1; i < 11; i++) {
		const int inputLength = layers[i - 1];
		const int outputLength = layers[i];
		const double sum = calcSum(arrIterable, inputLength);
    double* output = new double[outputLength];

    #pragma omp parallel for if (PARALLEL)
    for (int u = 0; u < outputLength; u++) {
      const double weight = distribution(generator);
      const double a = weight * sum + 1.0;
      const double expPlus = exp(a);
	    const double expMinus = exp(-a);
	    output[u] = (expPlus - expMinus) / (expPlus + expMinus);
    }

		delete[] arrIterable;
		arrIterable = output;
	}

  const double end = omp_get_wtime();
  std::cout << "\nOutput array:\n";
  showArr(arrIterable, layers[ARR_LENGTH]);
  const double duration = end - start;
  std::cout << "\nDuration: " << duration * 1000 << " ms.\n";

  return EXIT_SUCCESS;
}
