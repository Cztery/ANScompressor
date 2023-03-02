#pragma once
#include <cuda.h>

#include <iostream>

template <typename T>
void check(T err, char const* const func, char const* const file,
           int const line) {
  if (err != cudaSuccess) {
    std::cerr << "CUDA Runtime Error at: " << file << ":" << line << std::endl;
    std::cerr << cudaGetErrorString(err) << " " << func << std::endl;
    std::exit(EXIT_FAILURE);
  }
}
#define CHECK_CUDA_ERROR(val) check((val), #val, __FILE__, __LINE__)

void checkLast(char const* const file, int const line) {
  cudaError_t err{cudaGetLastError()};
  if (err != cudaSuccess) {
    std::cerr << "CUDA Runtime Error at: " << file << ":" << line << std::endl;
    std::cerr << cudaGetErrorString(err) << std::endl;
    std::exit(EXIT_FAILURE);
  }
}
#define CHECK_LAST_CUDA_ERROR() checkLast(__FILE__, __LINE__)