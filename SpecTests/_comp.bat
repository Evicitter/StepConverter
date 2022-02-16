g++ -O3 -std=c++11 -march=nehalem -fopenmp -static -c base.cpp -o base.o
clang++ -O3 -Oz -std=c++11 -march=nehalem -fopenmp Base.o %1.cpp -o %1.exe
del base.o