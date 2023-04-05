#include "test.h"

// We want to continue to use our Halide::Buffer with AOT-compiled
// code, so we explicitly include it. It's a header-only class, and
// doesn't require libHalide.
#include "HalideBuffer.h"

#include <stdio.h>

#define SIZE 512

int main(int argc, char **argv) {
	Halide::Runtime::Buffer<int32_t> output(SIZE);

	// Halide::Runtime::Buffer also has constructors that wrap
	// existing data instead of allocating new memory. Use these if
	// you have your own Image type that you want to use.

	int ret = func(output);

	if (ret) {
		printf("Halide returned an error: %d\n", ret);
		return -1;
	}

	// Force any accelerator code to finish by copying the buffer back to the CPU.
	output.copy_to_host();

	bool success = true;

	for (int x = 0; x < SIZE; x++) {
		uint32_t output_val = output(x);
		uint32_t correct_val = x;
		if (output_val != correct_val) {
			printf("output(%d) was %d instead of %d\n", x, output_val, correct_val);
			success = false;
			//return -1;
		}
	}

	if (success) {
		// Everything worked!
		printf("Success!\n");
	} else {
		printf("There were mismatches :(\n");
	}
	return 0;
}
