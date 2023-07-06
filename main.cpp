#include "Halide.h"
#include <stdio.h>

int main(int argc, char **argv)
{
    Halide::Func output;
    Halide::Var x;

    output(x) = x + 42;

    output.compile_to_mlir("halide_mlir_test", {});

    return 0;
}
