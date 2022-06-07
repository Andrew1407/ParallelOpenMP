#include <iostream>
#include <thread>

#ifdef _DEBUG
#undef _DEBUG
#include <omp.h>
#define _DEBUG
#else
#include <omp.h>
#endif

using namespace std;

int main()
{
	const char const *outputStr = "Hello world, my name is Golovko Andrew Andreevich!!!";
	const unsigned int processor_count = std::thread::hardware_concurrency();

	omp_set_num_threads(processor_count);
	#pragma omp parallel
	{
		#pragma omp critical
		cout << outputStr << endl;
	}

	return EXIT_SUCCESS;
}
