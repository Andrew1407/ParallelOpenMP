#pragma once
#include<stdio.h>
#include<math.h>
#include <vector>
#include <fstream>
#include <omp.h>
#include <iostream>

using namespace std;

#define M_PI 3.14159265358979323846
#define THETA (M_PI / 3)
#define THETA_COS cos(THETA)
#define THETA_SIN sin(THETA)

typedef pair<double, double> point;
typedef pair<point, point> line;

class KochSnowflake {
  public:
    KochSnowflake(const int width, const int height) {
      fractalSize = make_pair(width, height);
    }

    double calcFractalPoints(const int depth, const bool parallelModifier = false) {
      points.clear();
      if (isParallel != parallelModifier)
        isParallel = parallelModifier;

      const double startTime = omp_get_wtime();
      const line initialLine = getInitialPoints();

      if (depth) {
        fractalize(depth, initialLine);
      } else {
        points.push_back(initialLine.first);
        points.push_back(initialLine.second);
      }

      return omp_get_wtime() - startTime;
    }

    void drawSVG(const char* filename) {
      ofstream out(filename);
      if (!out) return;
      out << "<svg xmlns='http://www.w3.org/2000/svg' width='"
        << fractalSize.first << "' height='" << fractalSize.second << "'>\n";
      out << "<rect width='100%' height='100%' fill='black'/>\n";
      out << "<path stroke-width='0.5' stroke='white' fill='none' d='";
      for (int i = 0, n = points.size(); i < n; ++i)
        out << (i ? 'L' : 'M') << points[i].first << ',' << points[i].second << '\n';
      out << "z'/>\n</svg>\n";
    }

  private:
    vector<point> points;
    pair<int, int> fractalSize;
    bool isParallel = false;

    line getInitialPoints() {
      const int lastX =  fractalSize.second - 10;
      const point a = make_pair(10, lastX);
      const point b = make_pair(fractalSize.first - 10, lastX);
      return make_pair(a, b);
    }

    void fractalize(const int depth, const line currentLine) {
      const point *p1 = &currentLine.first;
      const point *p2 = &currentLine.second;

      if (!depth) {
        points.push_back(currentLine.first);
        points.push_back(currentLine.second);
        return;
      }

      const point p3 = make_pair(
        (2 * p1->first + p2->first) / 3,
        (2 * p1->second + p2->second) / 3
      );
      const point p5 =make_pair(
        (2 * p2->first + p1->first) / 3,
        (2 * p2->second + p1->second) / 3
      );
      const double p4X = p5.first - p3.first;
      const double p4Y = p5.second - p3.second;
      const point p4 = make_pair(
        p3.first + p4X * THETA_COS + p4Y * THETA_SIN,
        p3.second - p4X * THETA_SIN + p4Y * THETA_COS
      );

      const line line1 = make_pair(*p1, p3);
      const line line2 = make_pair(p3, p4);
      const line line3 = make_pair(p4, p5);
      const line line4 = make_pair(p5, *p2);

      #pragma omp task shared(points) if (isParallel && depth > 1)
      fractalize(depth - 1, line1);
      #pragma omp task shared(points) if (isParallel && depth > 1)
      fractalize(depth - 1, line2);
      #pragma omp task shared(points) if (isParallel && depth > 1)
      fractalize(depth - 1, line3);
      #pragma omp task shared(points) if (isParallel && depth > 1)
      fractalize(depth - 1, line4);
    }
};
