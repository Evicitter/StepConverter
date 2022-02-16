SET OPT_ADVPARAM=-time-passes -march=nehalem -code-model=large -regalloc=default -o fileIn.bc
(clang++ %1.cpp -c -std=c++11 -Xclang -disable-llvm-optzns -Xclang -disable-llvm-passes -Xclang -disable-llvm-verifier -Xclang -emit-llvm-bc -march=nehalem -o fileIn.bc) && (
opt fileIn.bc -cfl-anders-aa -cfl-steens-aa -tbaa -scoped-noalias -sroa -early-cse -instsimplify %OPT_ADVPARAM% ) && (
:opt fileIn.bc -gvn-hoist %OPT_ADVPARAM%
opt fileIn.bc -lower-expect -lowerswitch -forceattrs -ipsccp -globalopt -mem2reg -deadargelim -instcombine -prune-eh -inline %OPT_ADVPARAM% ) && (
opt fileIn.bc -argpromotion -sroa -early-cse -jump-threading -mldst-motion %OPT_ADVPARAM% ) && (
:opt fileIn.bc -correlated-propagation %OPT_ADVPARAM% ) && (
opt fileIn.bc -simplifycfg -instcombine %OPT_ADVPARAM% ) && (
opt fileIn.bc -reassociate -licm -simplifycfg -indvars %OPT_ADVPARAM% ) && (
opt fileIn.bc -gvn -memcpyopt -sccp -bdce %OPT_ADVPARAM% ) && (
opt fileIn.bc -die -instcombine -jump-threading %OPT_ADVPARAM% ) && (
:opt fileIn.bc -correlated-propagation %OPT_ADVPARAM% ) && (
opt fileIn.bc -dse %OPT_ADVPARAM% ) && (
opt fileIn.bc -licm -load-combine -adce -simplifycfg -instcombine %OPT_ADVPARAM% ) && (
:opt fileIn.bc -float2int %OPT_ADVPARAM% ) && (
opt fileIn.bc -alignment-from-assumptions -strip-dead-prototypes %OPT_ADVPARAM% ) && (
opt fileIn.bc -globaldce -constmerge -mergefunc %OPT_ADVPARAM% ) && (
opt fileIn.bc -unreachableblockelim %OPT_ADVPARAM% ) && (
clang++ fileIn.bc -c -std=c++11 -Xclang -disable-llvm-optzns -Xclang -disable-llvm-passes -Xclang -disable-llvm-verifier -Xclang -emit-obj -march=nehalem -o fileOut.obj  ) && (
del fileIn.bc  ) && (
clang++ fileOut.obj -o %1.exe  ) && (
del fileOut.obj  )
SET OPT_ADVPARAM=