#include <cstdio>
__global__ void mykernel() {

}

int main() {
    mykernel <<<1,1>>>(); //launch mykernel on GPU
    printf("Hello GPU\n");
    return 0;
}