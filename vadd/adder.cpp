#include <ap_int.h>

#define DATA_SIZE 4096

extern "C" {
/*
    Vector Addition Kernel Implementation using dataflow
    Arguments:
        in   (input)  --> Input Vector
        out  (output) --> Output Vector
        inc  (input)  --> Increment
        size (input)  --> Size of Vector in Integer
   */
void adder(unsigned int* in1, unsigned int* in2, unsigned int* out, int size) {
#pragma HLS INTERFACE m_axi port = in1 bundle = gmem0 channel
#pragma HLS INTERFACE m_axi port = in2 bundle = gmem0 channel
#pragma HLS INTERFACE m_axi port = out bundle = gmem0 channel
#pragma HLS INTERFACE s_axilite port = size
#pragma HLS INTERFACE s_axilite port = return
// Auto-pipeline is going to apply pipeline to this loop
vadd:
    for (int i = 0; i < size; i++) {
        out[i] = in1[i] + in2[i];
    }
}

}