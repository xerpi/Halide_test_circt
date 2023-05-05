#include <cstdio>
#include <chrono>
#include "HalideBuffer.h"
#include "test_blur_3x3_sliding_window_host.h"

int main(int argc, char **argv) {
    int size_x = 64;
    int size_y = 64;
    int num_runs = 1;
    int dump = 0;

    if (argc > 1)
        size_x = atoi(argv[1]);
    if (argc > 2)
        size_y = atoi(argv[2]);
    if (argc > 3)
        num_runs = atoi(argv[3]);
    if (argc > 4)
        dump = atoi(argv[4]);

    printf("size_x: %d\n", size_x);
    printf("size_y: %d\n", size_y);
    printf("num_runs: %d\n", num_runs);
    printf("dump: %d\n", dump);

    int32_t *input_buffer = new int32_t[size_x * size_y];

    for (int i = 0; i < size_y; i++)
        for (int j = 0; j < size_x; j++)
            input_buffer[j + i * size_x] = i + j;

    Halide::Runtime::Buffer<int32_t, 2> input((int32_t *)input_buffer, size_x, size_y);
    Halide::Runtime::Buffer<int32_t, 2> output(size_x-2, size_y-2);

    output.set_min(1, 1);
    input.set_host_dirty();

    auto start = std::chrono::high_resolution_clock::now();

    int ret;
    for (int i = 0; i < num_runs; i++)
        ret = test_blur_3x3_sliding_window(input, output);

    auto finish = std::chrono::high_resolution_clock::now();

    if (ret) {
        fprintf(stderr, "Halide returned an error: %d\n", ret);
        return -1;
    }

    // Force any accelerator code to finish by copying the buffer back to the CPU.
    output.copy_to_host();

    printf("Done!\n");

    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count();
    printf("Duration (host): %ld ns\n", duration);

    // Dump output
    if (dump) {
        char file[512];
        snprintf(file, sizeof(file), "%s_out.bin", argv[0]);
        FILE *fp = fopen(file, "wb");
        assert(fp);
        fwrite(output.data(), 1, output.size_in_bytes(), fp);
        fclose(fp);

        printf("Dumped %zu bytes to '%s'\n", output.size_in_bytes(), file);
    }

    delete[] input_buffer;

    return 0;
}
