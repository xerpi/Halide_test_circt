```bash
cmake .. -DCMAKE_PREFIX_PATH=$PWD/../../Halide/install -DXRT_DIR=$XILINX_XRT/share/cmake/XRT/
```

```bash
cmake .. \
  -DHalide_DIR=$PWD/../../Halide/install/lib/cmake/Halide \
  -DHalideHelpers_DIR=$PWD/../../Halide/install/lib/cmake/HalideHelpers \
  -DLLVM_DIR=$PWD/../../circt/llvm/build/lib/cmake/llvm \
  -DLLVM_ENABLE_ASSERTIONS=ON \
  -DCIRCT_DIR=$PWD/../../circt/build/lib/cmake/circt/ \
  -DMLIR_DIR=$PWD/../../circt/llvm/build/lib/cmake/mlir/
```

```bash
cmake .. \
  -DHalide_DIR=$PWD/../../Halide/build/packaging \
  -DHalideHelpers_DIR=$PWD/../../Halide/build/packaging \
  -DLLVM_DIR=$PWD/../../circt/llvm/build/lib/cmake/llvm \
  -DLLVM_ENABLE_ASSERTIONS=ON \
  -DCIRCT_DIR=$PWD/../../circt/build/lib/cmake/circt/ \
  -DMLIR_DIR=$PWD/../../circt/llvm/build/lib/cmake/mlir/
```
