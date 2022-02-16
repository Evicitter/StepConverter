SET OPT_ADVPARAM=-time-passes -march=nehalem -code-model=large -regalloc=default -o fileIn.bc
(clang++ %1.cpp -c -std=c++11 -Xclang -disable-llvm-optzns -Xclang -disable-llvm-passes -Xclang -disable-llvm-verifier -Xclang -emit-llvm-bc -march=nehalem -o fileIn.bc) && (
opt fileIn.bc -simplifycfg -sroa -early-cse -lower-expect %OPT_ADVPARAM% ) && (
:opt fileIn.bc -gvn-hoist %OPT_ADVPARAM%
opt fileIn.bc -lower-expect %OPT_ADVPARAM% ) && (
opt fileIn.bc -forceattrs -ipsccp -globalopt -mem2reg -deadargelim -instcombine %OPT_ADVPARAM% ) && (
clang++ fileIn.bc -c -std=c++11 -Xclang -disable-llvm-optzns -Xclang -disable-llvm-passes -Xclang -disable-llvm-verifier -Xclang -emit-obj -march=nehalem -o fileOut.obj  ) && (
del fileIn.bc  ) && (
clang++ fileOut.obj -o %1.exe  ) && (
del fileOut.obj  )