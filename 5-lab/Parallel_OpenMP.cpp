#include <iostream>
#include <omp.h>

using namespace std;

#define RANGE 10000

typedef struct {
	float sum1 = 0;
	float sum2 = 0;
	float sum3_1 = 0;
	float sum3_2 = 0;
	float sum4_1 = 0;
	float sum4_2 = 0;

	void clear() {
		sum1 = sum2 = sum3_1 = sum3_1 = sum3_2 = sum4_1 = sum4_2 = 0;
	}

	void log() {
		cout << "sum1 = " << sum1 << endl << "sum2 = " << sum2 << endl
			<< "sum3_1 = " << sum3_1 << "\tsum3_2 = " << sum3_2
			<< "\tsum3_1 + sum3_2 = " << sum3_1 + sum3_2 << endl
			<< "sum4_1 = " << sum4_1 << "\tsum4_2 = " << sum4_2
			<< "\tsum4_1 + sum4_2 = " << sum4_1 + sum4_2 << endl;
	}

} sums;


double calcSumsSeq(sums* s) {
	const double start = omp_get_wtime();
	for (int i = 0; i < RANGE; i++) {
		const float el = 1.0 / (i + 1);
		const float elSub = 1.0 / (RANGE - i);
		const bool isEven = (i + 1) % 2 == 0;
		float* const halfSum3 = isEven ? &s->sum3_1 : &s->sum3_2;
		float* const halfSum4 = isEven ? &s->sum4_1 : &s->sum4_2;
		s->sum1 += el;
		s->sum2 += elSub;
		*halfSum3 += elSub;
		*halfSum4 += el;
	}

	return omp_get_wtime() - start;
}

double calcSumsPar(sums* s) {
	float sum1 = 0;
	float sum2 = 0;
	float sum3_1 = 0;
	float sum3_2 = 0;
	float sum4_1 = 0;
	float sum4_2 = 0;

	const double start = omp_get_wtime();
	#pragma omp simd reduction(+: sum1, sum2, sum3_1, sum3_2, sum4_1, sum4_2)
	for (int i = 0; i < RANGE; i++) {
		const float el = 1.0 / (i + 1);
		const float elSub = 1.0 / (RANGE - i);
		const bool isEven = (i + 1) % 2 == 0;
		float* halfSum3 = isEven ? &sum3_1 : &sum3_2;
		float* halfSum4 = isEven ? &sum4_1 : &sum4_2;
		sum1 += el;
		sum2 += elSub;
		*halfSum3 += elSub;
		*halfSum4 += el;
	}
	const double end = omp_get_wtime();

	s->sum1 = sum1;
	s->sum2 = sum2;
	s->sum3_1 = sum3_1;
	s->sum3_2 = sum3_2;
	s->sum4_1 = sum4_1;
	s->sum4_2 = sum4_2;

	return end - start;
}

int main()
{
	omp_set_num_threads(12);

	sums s;
	const double seqTime = calcSumsSeq(&s);
	cout << "Sequentially (" << seqTime * 1000 << " ms.):\n";
	s.log();
	s.clear();
	const double parTime = calcSumsPar(&s);
	cout << "\nParallel (" << parTime * 1000 << " ms.):\n";
	s.log();

	return EXIT_SUCCESS;
}
