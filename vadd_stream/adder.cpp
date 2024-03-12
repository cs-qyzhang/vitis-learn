#include <ap_int.h>
#include <hls_stream.h>

#define DATA_SIZE 4096

void add(hls::stream<int>& in1, hls::stream<int>& in2, hls::stream<int>& out, int size) {
    for (int i = 0; i < size; i++) {
        out.write(in1.read() + in2.read());
    }
}

void load(hls::stream<int>& in1, hls::stream<int>& in2, int* in1_data, int* in2_data, int size) {
    for (int i = 0; i < size; i++) {
        in1.write(in1_data[i]);
        in2.write(in2_data[i]);
    }
}

void store(hls::stream<int>& out, int* out_data, int size) {
    for (int i = 0; i < size; i++) {
        out_data[i] = out.read();
    }
}

extern "C" {
/*
    Vector Addition Kernel Implementation using dataflow
    Arguments:
        in   (input)  --> Input Vector
        out  (output) --> Output Vector
        inc  (input)  --> Increment
        size (input)  --> Size of Vector in Integer
   */
void adder(int* in1, int* in2, int* out, int size) {
#pragma HLS INTERFACE m_axi port = in1 bundle = gmem0 channel
#pragma HLS INTERFACE m_axi port = in2 bundle = gmem0 channel
#pragma HLS INTERFACE m_axi port = out bundle = gmem0 channel

#pragma HLS DATAFLOW
    hls::stream<int> in1_stream;
    hls::stream<int> in2_stream;
    hls::stream<int> out_stream;

    load(in1_stream, in2_stream, in1, in2, size);
    add(in1_stream, in2_stream, out_stream, size);
    store(out_stream, out, size);
}

}