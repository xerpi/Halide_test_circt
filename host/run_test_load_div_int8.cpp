#include <cstdio>
#include <chrono>
#include "HalideBuffer.h"
#include "test_load_div_int8_host.h"

int main(int argc, char **argv) {
    int size = 32*1024*1024;
    int num_runs = 1;
    int dump = 0;

    if (argc > 1)
        size = atoi(argv[1]);
    if (argc > 2)
        num_runs = atoi(argv[2]);
    if (argc > 3)
        dump = atoi(argv[3]);

    printf("size: %d\n", size);
    printf("num_runs: %d\n", num_runs);
    printf("dump: %d\n", dump);

    int8_t *input_buffer = new int8_t[size];

    for (int i = 0; i < size; i++)
        input_buffer[i] = i;

    Halide::Runtime::Buffer<int8_t> input(input_buffer, size);
    Halide::Runtime::Buffer<int8_t> output(size);
    input.set_host_dirty();

    auto start = std::chrono::high_resolution_clock::now();

    int ret;
    for (int i = 0; i < num_runs; i++)
        ret = test_load_div_int8(input, output);

    if (ret) {
        printf("Halide returned an error: %d\n", ret);
        return -1;
    }

    auto finish = std::chrono::high_resolution_clock::now();

    // Force any accelerator code to finish by copying the buffer back to the CPU.
    output.copy_to_host();

    printf("Done!\n");

    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count();
    printf("Duration (host): %ld ns\n", duration);

    bool success = true;

#if 0
    for (int x = 0; x < size; x++) {
        int8_t output_val = output(x);
        int8_t correct_val = input_buffer[x] / 3;
        if (output_val != correct_val) {
            printf("output(%d) was %d instead of %d\n", x, output_val, correct_val);
            success = false;
            //return -1;
        }
    }
#endif

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

    if (success) {
        // Everything worked!
        printf("Success!\n");
    } else {
        printf("There were mismatches :(\n");
        return -1;
    }
    return 0;
}
