#include <stdio.h>
#include "HalideBuffer.h"
#include "test_load_host.h"

#define SIZE 512

int main(int argc, char **argv) {
	int32_t input_buffer[SIZE];

	for (int i = 0; i < SIZE; i++)
		input_buffer[i] = i;

	Halide::Runtime::Buffer<int32_t> input(input_buffer, SIZE);
	Halide::Runtime::Buffer<int32_t> output(SIZE);

	int ret = test_load(input, output);

	if (ret) {
		printf("Halide returned an error: %d\n", ret);
		return -1;
	}

	// Force any accelerator code to finish by copying the buffer back to the CPU.
	output.copy_to_host();

	bool success = true;

	for (int x = 0; x < SIZE; x++) {
		uint32_t output_val = output(x);
		uint32_t correct_val = 2 * input_buffer[x];
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
