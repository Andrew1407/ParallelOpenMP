if [ $# -eq 0 ]
then
  c++ -o out -fopenmp Parallel_OpenMP.cpp
else
  icc $@ -fopenmp Parallel_OpenMP.cpp -o out
fi
