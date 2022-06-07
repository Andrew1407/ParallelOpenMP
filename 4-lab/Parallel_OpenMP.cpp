#include "koch.cpp"
#include "hanoi.cpp"
#include <iostream>
#include <omp.h>

#define IS_PARALLEL true
#define SHOW_LOGS true
#define THREADS_AMOUNT 8

void showExecution(const int step, const double seqTime, const double parTime) {
  std::cout << "Depth " << step << ": "
    << seqTime * 1000 << " ms. (sequential); "
    << parTime * 1000 << " ms. (parallel);\n";
}

void testKochSnowflake() {
  const int svgWidth = 1500;
  const int svgHeight = 500;
  KochSnowflake ks(svgWidth, svgHeight);
  const int depthToDraw = 7;

  std::cout << "Koch snowflake" << std::endl;
  for (int i = 0; i < 14; i++) {
    const double timeSequential = ks.calcFractalPoints(i);
    const double timeParallel = ks.calcFractalPoints(i, IS_PARALLEL);
    if (i == depthToDraw) ks.drawSVG("koch.svg");
    showExecution(i, timeSequential, timeParallel);
  }
}

void testHanoiTower() {
  const char inputRod = 'A', outputRod = 'B', middleRod = 'C';
  HanoiTower ht;
  const int exampleHeight = 4;
  std::cout << "Hanoi tower" << std::endl;
  std::cout << "Example (height: " << exampleHeight << ")" << std::endl;
  ht.setParallel(IS_PARALLEL);
  ht.setShowLogs(SHOW_LOGS);
  ht.runCycle(exampleHeight, inputRod, outputRod, middleRod);
  std::cout << std::endl;

  ht.setShowLogs(!SHOW_LOGS);
  for (int i = 5; i < 28; i++) {
    ht.setParallel(!IS_PARALLEL);
    const double timeSequential = ht.runCycle(i, inputRod, outputRod, middleRod);
    ht.setParallel(IS_PARALLEL);
    const double timeParallel = ht.runCycle(i, inputRod, outputRod, middleRod);
    showExecution(i, timeSequential, timeParallel);
  }
}

int main() {
  omp_set_num_threads(THREADS_AMOUNT);
  testKochSnowflake();
  std::cout << std::endl;
  testHanoiTower();

  return EXIT_SUCCESS;
}
