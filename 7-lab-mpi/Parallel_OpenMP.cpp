#include <iostream>
#include <time.h>
#include <mpi.h>

using namespace std;

#define ARRAY_SZIE 10000

void generateElements(int* arr, const int rank) {
    const int halfSize = ARRAY_SZIE / 2;
    for (int i = 0; i < ARRAY_SZIE; i++) {
        arr[i] = rand() % 100;
        if (rank && arr[i] == 0) arr[i]++;
        if (rank && i > halfSize || !rank && i < halfSize) arr[i] *= 2;
        else if (rank && i < halfSize || !rank && i > halfSize) arr[i] /= 2;
    }
}

int main(int argc, char **argv) {
    srand(time(NULL));
    int worldRank;
    MPI_Status status;
    const int halfSize = ARRAY_SZIE / 2;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);

    if (worldRank == 1) {
        int a[ARRAY_SZIE];
        int bFirstHalf[halfSize];
        generateElements(a, 1);
        const double start = MPI_Wtime();
        MPI_Send(&a[halfSize], halfSize, MPI_INT, 0, 1, MPI_COMM_WORLD);
        MPI_Recv(&bFirstHalf[0], halfSize, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
        for (int i = 0; i < halfSize; i++) {
            const int sum = a[i] + bFirstHalf[i];
            cout << "a[" << i << "] + b[" << i << "] = " << a[i]
                << " + " << bFirstHalf[i] << " = " << sum << endl;
        }
        const double timeElapsed = (MPI_Wtime() - start) * 1000;
        cout << "Time elapsed (rank " << worldRank << "): " << timeElapsed << " ms." << endl;
    } else if (worldRank == 0) {
        int b[ARRAY_SZIE];
        int aSecondHalf[halfSize];
        generateElements(b, 0);
        const double start = MPI_Wtime();
        MPI_Recv(&aSecondHalf[0], halfSize, MPI_INT, 1, 1, MPI_COMM_WORLD, &status);
        MPI_Send(&b[0], halfSize, MPI_INT, 1, 2, MPI_COMM_WORLD);
        for (int i = 0; i < halfSize; i++) {
            const int secondHalfIdx = i + halfSize;
            const int sum = aSecondHalf[i] + b[secondHalfIdx];
            cout << "a[" << secondHalfIdx << "] + b[" << secondHalfIdx << "] = "
                << aSecondHalf[i] << " + " << b[secondHalfIdx] << " = " << sum << endl;
        }
        const double timeElapsed = (MPI_Wtime() - start) * 1000;
        cout << "Time elapsed (rank " << worldRank << "): " << timeElapsed << " ms." << endl;
    }
    
    MPI_Finalize();
    return EXIT_SUCCESS;
}
