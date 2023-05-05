// Halide tutorial lesson 1: Getting started with Funcs, Vars, and Exprs

// This lesson demonstrates basic usage of Halide as a JIT compiler for imaging.

// On linux, you can compile and run it like so:
// g++ lesson_01*.cpp -g -I <path/to/Halide.h> -L <path/to/libHalide.so> -lHalide -lpthread -ldl -o lesson_01 -std=c++11
// LD_LIBRARY_PATH=<path/to/libHalide.so> ./lesson_01

// On os x:
// g++ lesson_01*.cpp -g -I <path/to/Halide.h> -L <path/to/libHalide.so> -lHalide -o lesson_01 -std=c++11
// DYLD_LIBRARY_PATH=<path/to/libHalide.dylib> ./lesson_01

// If you have the entire Halide source tree, you can also build it by
// running:
//    make tutorial_lesson_01_basics
// in a shell with the current directory at the top of the halide
// source tree.

// The only Halide header file you need is Halide.h. It includes all of Halide.
#include "Halide.h"

// We'll also include stdio for printf.
#include <stdio.h>

void test_store(Halide::Target target)
{
    Halide::Func func;
    Halide::Var x, y;

    func(x, y) = x + y;

    //func.vectorize(x, 2);

    std::string name(__FUNCTION__);
    func.compile_to_file(name +  "_host", {}, name, target);
}

void test_store_offset(Halide::Target target)
{
    Halide::Func output;
    Halide::Var x;

    output(x) = x + 42;

    //func.vectorize(x, 2);

    std::string name(__FUNCTION__);
    output.compile_to_file(name +  "_host", {}, name, target);
}

void test_load(Halide::Target target)
{
    Halide::ImageParam in(Halide::type_of<int32_t>(), 1);
    Halide::Func func;
    Halide::Var x;

    func(x) = 5 * in(x);

    // AXI data bus = 1024b -> (1024/8)/4 = 32 elements
    func.vectorize(x, 32);

    //func.unroll(x, 32);
    std::string name(__FUNCTION__);
    func.compile_to_file(name +  "_host", {in}, name, target);
}

void test_load_div_int8(Halide::Target target)
{
    Halide::ImageParam in(Halide::type_of<int8_t>(), 1);
    Halide::Func func;
    Halide::Var x;

    func(x) = in(x) / 3;

    // AXI data bus = 1024b -> (1024/8)/1 = 128 elements
    func.vectorize(x, 128);

    //func.unroll(x, 32);
    std::string name(__FUNCTION__);
    func.compile_to_file(name +  "_host", {in}, name, target);
}

void test_gradient(Halide::Target target)
{
    Halide::ImageParam in(Halide::type_of<int32_t>(), 2);
    Halide::Func func;
    Halide::Var x, y;
    Halide::Var x_outer, y_outer, x_inner, y_inner, tile_index;

    func(x, y) = x + y;

    func.tile(x, y, x_outer, y_outer, x_inner, y_inner, 64, 64)
        .fuse(x_outer, y_outer, tile_index);

    std::string name(__FUNCTION__);
    func.compile_to_file(name +  "_host", {in}, name, target);
}

void test_blur_3x3(Halide::Target target)
{
    Halide::ImageParam input(Halide::type_of<int32_t>(), 2);
    Halide::Func blur_x, blur_y;
    Halide::Var x, y, xi, yi;

    // The algorithm - no storage or order
    blur_x(x, y) = (input(x-1, y) + input(x, y) + input(x+1, y))/3;
    blur_y(x, y) = (blur_x(x, y-1) + blur_x(x, y) + blur_x(x, y+1))/3;

    // The schedule - defines order, locality; implies storage
    blur_y.tile(x, y, xi, yi, 16, 16);
    //blur_y.vectorize(xi, 8);

    blur_x.store_at(blur_y, x)
          .compute_at(blur_y, yi);
    //blur_x.vectorize(x, 8);

    std::string name(__FUNCTION__);
    blur_y.compile_to_file(name +  "_host", {input}, name, target);
}

void test_blur_3x3_sliding_window(Halide::Target target)
{
    Halide::ImageParam input(Halide::type_of<int32_t>(), 2);
    Halide::Func blur_x, blur_y, tmp;
    Halide::Var x, y, xi, yi;

    // The algorithm - no storage or order
    tmp(x, y) = input(x, y);
    blur_x(x, y) = (tmp(x-1, y) + tmp(x, y) + tmp(x+1, y))/3;
    blur_y(x, y) = (blur_x(x, y-1) + blur_x(x, y) + blur_x(x, y+1))/3;

    // The schedule - defines order, locality; implies storage
    blur_y.tile(x, y, xi, yi, 8, 8);
    //blur_y.vectorize(xi, 8);

    blur_x.store_at(blur_y, x)
          .compute_at(blur_y, yi);

    tmp.store_at(blur_x, y)
       .compute_at(blur_x, x);

    // For CPUs
    // blur_y.tile(x,y,xi,yi,256,32).vectorize(xi,8).parallel(y);
    // blur_x.compute_at(blur_y,x).store_at(blur_y,x).vectorize(x,8);

    //blur_x.vectorize(x, 8);

    std::string name(__FUNCTION__);
    blur_y.compile_to_file(name +  "_host", {input}, name, target);
}

Halide::Func test_simple()
{
    Halide::Func gradient;
    Halide::Var x, y;
    Halide::Expr e = x + y;

    gradient(x, y) = e;

    //gradient.unroll(x, 32);

    return gradient;
}

Halide::Func test_simple2()
{
    Halide::Func gradient, out;
    Halide::Var x, y;
    Halide::Expr e = x + y;

    gradient(x, y) = e;

    out(x, y) = gradient(x, y) * 2;

    //gradient.unroll(x, 32);

    return out;
}

Halide::Func blur_3x3(Halide::Func input)
{
  Halide::Func blur_x, blur_y;
  Halide::Var x, y, xi, yi;

  // The algorithm - no storage or order
  blur_x(x, y) = (input(x-1, y) + input(x, y) + input(x+1, y))/3;
  blur_y(x, y) = (blur_x(x, y-1) + blur_x(x, y) + blur_x(x, y+1))/3;

  // The schedule - defines order, locality; implies storage
  blur_y.tile(x, y, xi, yi, 256, 32)
        /*.vectorize(xi, 8)*/.parallel(y);
  blur_x.compute_at(blur_y, x)/*.vectorize(x, 8)*/;

  return blur_y;
}


Halide::Func test_filter()
{
    int array[32][32];
    for (int i = 0; i < 32; i++)
        for (int j = 0; j < 32; j++)
            array[i][j] = i + j;

    Halide::Buffer<int> in(array);

    Halide::Func bx, by;
    Halide::Var x, y, xi, yi;

    // The algorithm
    bx(x, y) = (in(x-1, y) + in(x, y) + in(x+1, y))/3;
    by(x, y) = (bx(x, y-1) + bx(x, y) + bx(x, y+1))/3;

    // The schedule
    /*by.tile(x, y, xi, yi, 256, 32)
      .vectorize(xi, 8)
      .parallel(y);
    bx.compute_at(by, x).
       vectorize(x, 8);*/

    /*by.tile(x, y, xi, yi, 256, 32)
      .parallel(y);
    bx.compute_at(by, x);*/

    //Halide::Var y_outer, y_inner;
    //bx.compute_at(by, x);
    //by.split(y, y_outer, y_inner, 4).unroll(y_inner);

    //bx.compute_at(by, y);

    //bx.compute_at(by, y);
    //bx.store_root();

    return by;
}

int main(int argc, char **argv)
{
    Halide::Target target = Halide::get_target_from_environment();
    std::cout << "Target: " << target.to_string() << std::endl;
    std::cout << "Has CIRCT feature: " << target.has_feature(Halide::Target::Feature::CIRCT) << std::endl;

    //test_store_offset(target);
    //test_load(target);
    //test_load_div_int8(target);
    //test_store(target);
    //test_gradient(target);
    //test_blur_3x3(target);
    test_blur_3x3_sliding_window(target);
    //Halide::Func func = test_store();
    //Halide::Func func = test_filter();
    //Halide::Func func = test_simple();

#if 0
    std::cout << "target.get_required_device_api(): " << int(target.get_required_device_api()) << std::endl;
    std::cout << "host_supports_target_device: " << host_supports_target_device(target) << std::endl;

    target.set_feature(Halide::Target::CIRCT);
    target.set_feature(Halide::Target::NoBoundsQuery);
    target.set_feature(Halide::Target::NoAsserts);

    std::cout << "Target: " << target.to_string() << std::endl;
    std::cout << "Has feature: " << target.has_feature(Halide::Target::Feature::CIRCT) << std::endl;
#endif

    //std::map<Halide::OutputFileType, std::string> files{{OutputFileType::c_source, "lesson_10_halide_stub.cpp"}};
    printf("Compile to files\n");

    //Param<uint8_t> offset;
    //Halide::ImageParam input(Halide::type_of<uint32_t>(), 1);

    //func.compile_to_file("test", {}, "func", target);
#if 0
    printf("Calling realize\n");

    //Halide::Var fused;
    //gradient.fuse(x, y, fused);

    //Halide::Buffer<int32_t> output gradient.realize_circt({800, 600}, target);
    Halide::Buffer<int32_t> output = func.realize({32}, target);
    //Halide::Buffer<int32_t> output = func.realize({800, 600}, target);

    //printf("Loop nest:\n");
    //gradient.print_loop_nest();

    // Halide does type inference for you. Var objects represent
    // 32-bit integers, so the Expr object 'x + y' also represents a
    // 32-bit integer, and so 'gradient' defines a 32-bit image, and
    // so we got a 32-bit signed integer image out when we call
    // 'realize'. Halide types and type-casting rules are equivalent
    // to C.

    printf("Checking output\n");

    // Let's check everything worked, and we got the output we were
    // expecting:
    for (int j = 0; j < output.height(); j++) {
        for (int i = 0; i < output.width(); i++) {
            // We can access a pixel of an Buffer object using similar
            // syntax to defining and using functions.
            if (output(i, j) != i + j) {
                printf("Something went wrong!\n"
                       "Pixel %d, %d was supposed to be %d, but instead it's %d\n",
                       i, j, i + j, output(i, j));
                return -1;
            }
        }
    }
#endif
    // Everything worked! We defined a Func, then called 'realize' on
    // it to generate and run machine code that produced an Buffer.
    printf("Success!\n");

    return 0;
}
