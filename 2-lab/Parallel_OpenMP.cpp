#include <iostream>
#include <omp.h>

using namespace std;

int main() {
	const int threads = 8;
	const int aLen = 16000;
	int a[aLen];
	int b[aLen];
	int chunks[threads];
	double fillTime[threads];
	double calcTime[threads];

	#pragma omp parallel num_threads(threads)
	{
		const int threadId = omp_get_thread_num();

		#pragma omp master
			for (int i = threads - 1, q = aLen; i >= 0; i--) {
				chunks[i] = q;
				q /= 2;
			}

		#pragma omp barrier

		const int chunkStartPosition = threadId == 0 ? 0 : chunks[threadId - 1];
		const int chunkEndPosition = chunks[threadId];
		const double fillStart = omp_get_wtime();
		for (int i = chunkStartPosition; i < chunkEndPosition; i++) a[i] = i;
		const double fillEnd = omp_get_wtime();
		fillTime[threadId] = fillEnd - fillStart;

		#pragma omp barrier
		const double calcStart = omp_get_wtime();
		for (int i = chunkStartPosition; i < chunkEndPosition; i++)
			if (i == 0 || i == aLen - 1)
				b[i] = 0;
			else
				b[i] = (a[i - 1] + a[i] + a[i + 1]) / 3;
		const double calcEnd = omp_get_wtime();
		calcTime[threadId] = calcEnd - calcStart;

		/*#pragma omp barrier
		#pragma omp master
			for (int i = 0; i < aLen; i++)
				cout << i << '\t' << a[i] << '\t' << b[i] << endl;*/
	}

	double totalFilled = 0.0;
	double totalCalculated = 0.0;
	const int secCoef = 10e9;
	for (int i = 0; i < threads; i++) {
		totalFilled += fillTime[i];
		totalCalculated += calcTime[i];
		const int rangeStart = i ? chunks[i - 1] : 0;
		const int rangeEnd = chunks[i];

		cout << "Total execution time for thread with " 
			<< rangeEnd - rangeStart << " ("<< rangeStart<< "-" << rangeEnd << ") elements:\n"
			<< "\tfilling time: " << fillTime[i] * secCoef << " ns.\n"
			<< "\tcalc. time: " << calcTime[i] * secCoef << " ns.\n"
			<< "\ttotal: " << (calcTime[i] + fillTime[i]) * secCoef << " ns.\n"
			<< endl;
	}

	cout << "Filling time (total): " << totalFilled * secCoef << " ns.\n"
		<< "Calc. time (total): " << totalCalculated * secCoef << " ns.\n"
		<< "Total (both): " << (totalCalculated + totalFilled) * secCoef << " ns."
		<< endl;

	return EXIT_SUCCESS;
}


