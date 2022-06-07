#pragma once
#include <iostream>
#include <omp.h>

class HanoiTower {
  public:
    void setShowLogs(const bool modifier) {
      showLogs = modifier;
    }

    void setParallel(const bool modifier) {
      isParallel = modifier;
    }

    double runCycle(const int height, const char inputRod, const char outputRod, const char middleRod) {
      const double start = omp_get_wtime();
      if (height) calculate(height, inputRod, outputRod, middleRod);
      return omp_get_wtime() - start;
    }

  private:
    bool showLogs = true;
    bool isParallel = false;

    void calculate(const int height, const char inputRod, const char outputRod, const char middleRod) {
      if (height < 2) {
        logMove(height, inputRod, outputRod);
        return;
      }

      #pragma omp task if (isParallel && height > 2)
      calculate(height - 1, inputRod, middleRod, outputRod);
      logMove(height, inputRod, outputRod);
      #pragma omp task if (isParallel && height > 2)
      calculate(height - 1, middleRod, outputRod, inputRod);
    }

    void logMove(const int disk, const char rodFrom, const char rodTo) {
      if (showLogs)
        std::cout << "Disk " << disk << " is moved from the rod "
          << rodFrom << " to the rod " << rodTo << std::endl; 
    }
};
