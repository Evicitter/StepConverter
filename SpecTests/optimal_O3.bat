SET OPT_ADVPARAM=-time-passes -march=nehalem -code-model=large -regalloc=greedy -o fileIn.bc
(clang++ %1.cpp -c -std=c++11 -momit-leaf-frame-pointer -Xclang -disable-llvm-optzns -Xclang -disable-llvm-passes -Xclang -disable-llvm-verifier -Xclang -emit-llvm-bc -march=nehalem -o fileIn.bc) && (
opt fileIn.bc -targetlibinfo -tti -verify -globalopt -demanded-bits -branch-prob -inferattrs -ipsccp -dse -loop-simplify -scoped-noalias -barrier -adce -deadargelim -memdep -licm -globals-aa -rpo-functionattrs -basiccg -loop-idiom -forceattrs -mem2reg -simplifycfg -early-cse -instcombine -sccp -loop-unswitch -loop-vectorize -tailcallelim -functionattrs -loop-accesses -memcpyopt -loop-deletion -reassociate -strip-dead-prototypes -loops -basicaa -lcssa -domtree -aa -block-freq -float2int -lower-expect -sroa -loop-unroll -alignment-from-assumptions -lazy-value-info -prune-eh -jump-threading -loop-rotate -indvars -bdce -scalar-evolution -tbaa -elim-avail-extern -mldst-motion -slp-vectorizer -gvn -inline -globaldce -constmerge -argpromotion -assumption-cache-tracker %OPT_ADVPARAM% ) && (
:opt fileIn.bc -unreachableblockelim %OPT_ADVPARAM% ) && (
clang++ fileIn.bc -c -std=c++11 -Xclang -disable-llvm-optzns -Xclang -disable-llvm-passes -Xclang -disable-llvm-verifier -Xclang -emit-obj -march=nehalem -o fileOut.obj  ) && (
del fileIn.bc  ) && (
clang++ fileOut.obj -o %1.exe  ) && (
del fileOut.obj  )
SET OPT_ADVPARAM=