#include <stdio.h>
#include "HalideBuffer.h"
#include "test_blur_3x3_host.h"

#define SIZE_X 512
#define SIZE_Y 512

int main(int argc, char **argv) {
    int32_t input_buffer[SIZE_Y][SIZE_X];

    for (int i = 0; i < SIZE_Y; i++)
        for (int j = 0; j < SIZE_X; j++)
            input_buffer[i][j] = i + j;

    Halide::Runtime::Buffer<int32_t, 2> input((int32_t *)input_buffer, SIZE_X, SIZE_Y);
    Halide::Runtime::Buffer<int32_t, 2> output(SIZE_X-2, SIZE_Y-2);

    output.set_min(1, 1);
    input.set_host_dirty();

    int ret = test_blur_3x3(input, output);

    if (ret) {
        fprintf(stderr, "Halide returned an error: %d\n", ret);
        return -1;
    }

    // Force any accelerator code to finish by copying the buffer back to the CPU.
    output.copy_to_host();

    // Dump output
    fwrite(output.data(), 1, output.size_in_bytes(), stdout);

    return 0;
}
