SET OPT_ADVPARAM=-time-passes -march=nehalem -code-model=large -regalloc=default -o fileIn.bc
(clang++ %1.cpp -c -std=c++11 -Xclang -disable-llvm-optzns -Xclang -disable-llvm-passes -Xclang -disable-llvm-verifier -Xclang -emit-llvm-bc -march=nehalem -o fileIn.bc) && (
opt fileIn.bc -cfl-anders-aa -cfl-steens-aa -tbaa -scoped-noalias -sroa -early-cse %OPT_ADVPARAM% ) && (
:opt fileIn.bc -gvn-hoist %OPT_ADVPARAM%
opt fileIn.bc -lower-expect %OPT_ADVPARAM% ) && (

:opt fileIn.bc -forceattrs -ipsccp %OPT_ADVPARAM%
opt fileIn.bc -ipsccp %OPT_ADVPARAM% ) && (
opt fileIn.bc -globalopt %OPT_ADVPARAM% ) && (
opt fileIn.bc -mem2reg %OPT_ADVPARAM% ) && (
opt fileIn.bc -deadargelim %OPT_ADVPARAM% ) && (
opt fileIn.bc -instcombine %OPT_ADVPARAM% ) && (
opt fileIn.bc -prune-eh %OPT_ADVPARAM% ) && (
opt fileIn.bc -inline %OPT_ADVPARAM% ) && (
opt fileIn.bc -argpromotion %OPT_ADVPARAM% ) && (
opt fileIn.bc -sroa -early-cse %OPT_ADVPARAM% ) && (
opt fileIn.bc -jump-threading %OPT_ADVPARAM% ) && (
opt fileIn.bc -correlated-propagation %OPT_ADVPARAM% ) && (
opt fileIn.bc -simplifycfg -instcombine %OPT_ADVPARAM% ) && (
opt fileIn.bc -reassociate %OPT_ADVPARAM% ) && (
opt fileIn.bc -licm %OPT_ADVPARAM% ) && (
opt fileIn.bc -simplifycfg %OPT_ADVPARAM% ) && (
opt fileIn.bc -indvars %OPT_ADVPARAM% ) && (
opt fileIn.bc -gvn %OPT_ADVPARAM% ) && (
opt fileIn.bc -memcpyopt %OPT_ADVPARAM% ) && (
opt fileIn.bc -sccp %OPT_ADVPARAM% ) && (
opt fileIn.bc -bdce %OPT_ADVPARAM% ) && (
opt fileIn.bc -instcombine %OPT_ADVPARAM% ) && (
opt fileIn.bc -jump-threading %OPT_ADVPARAM% ) && (
opt fileIn.bc -correlated-propagation %OPT_ADVPARAM% ) && (
opt fileIn.bc -dse %OPT_ADVPARAM% ) && (
opt fileIn.bc -licm %OPT_ADVPARAM% ) && (
opt fileIn.bc -load-combine %OPT_ADVPARAM% ) && (
opt fileIn.bc -adce %OPT_ADVPARAM% ) && (
opt fileIn.bc -simplifycfg %OPT_ADVPARAM% ) && (
opt fileIn.bc -instcombine %OPT_ADVPARAM% ) && (
:opt fileIn.bc -float2int %OPT_ADVPARAM% ) && (
opt fileIn.bc -alignment-from-assumptions %OPT_ADVPARAM% ) && (
opt fileIn.bc -strip-dead-prototypes %OPT_ADVPARAM% ) && (
opt fileIn.bc -globaldce %OPT_ADVPARAM% ) && (
opt fileIn.bc -constmerge %OPT_ADVPARAM% ) && (
:opt fileIn.bc -unreachableblockelim %OPT_ADVPARAM% ) && (
opt fileIn.bc -mergefunc %OPT_ADVPARAM% ) && (
clang++ fileIn.bc -c -std=c++11 -Xclang -disable-llvm-optzns -Xclang -disable-llvm-passes -Xclang -disable-llvm-verifier -Xclang -emit-obj -march=nehalem -o fileOut.obj  ) && (
del fileIn.bc  ) && (
clang++ fileOut.obj -o %1.exe  ) && (
del fileOut.obj  )
