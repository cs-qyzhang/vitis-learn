#include <ap_int.h>
#include <hls_stream.h>
#include <hls_print.h>

void compare(hls::stream<int>& in1_stream, hls::stream<int>& in2_stream, hls::stream<int>& out_stream, int size) {
    int in1_cnt = 0;
    int in2_cnt = 0;
    int in1_data = in1_stream.read();
    int in2_data = in2_stream.read();

    do {
        if (in1_data < in2_data) {
            out_stream.write(in1_data);
            in1_cnt++;
            if (in1_cnt < size)
                in1_data = in1_stream.read();
        } else {
            out_stream.write(in2_data);
            in2_cnt++;
            if (in2_cnt < size)
                in2_data = in2_stream.read();
        }
    } while (in1_cnt < size && in2_cnt < size);

    if (in1_cnt < size) {
        do {
            out_stream.write(in1_data);
            in1_cnt++;
            if (in1_cnt < size)
                in1_data = in1_stream.read();
        } while (in1_cnt < size);
    } else {
        do {
            out_stream.write(in2_data);
            in2_cnt++;
            if (in2_cnt < size)
                in2_data = in1_stream.read();
        } while (in2_cnt < size);
    }
}

void compare1(hls::stream<int>& in1_stream, hls::stream<int>& in2_stream, hls::stream<int>& out_stream, int size) {
    int in1_cnt = 0;
    int in2_cnt = 0;
    int in1_data = in1_stream.read();
    int in2_data = in2_stream.read();

    for (int i = 0; i < size * 2; i++) {
        if (in1_cnt == size) {
            out_stream.write(in2_data);
            in2_cnt++;
            if (in2_cnt < size)
                in2_data = in2_stream.read();
        } else if (in2_cnt == size) {
            out_stream.write(in1_data);
            in1_cnt++;
            if (in1_cnt < size)
                in1_data = in1_stream.read();
        } else if (in1_data < in2_data) {
            out_stream.write(in1_data);
            in1_cnt++;
            if (in1_cnt < size)
                in1_data = in1_stream.read();
        } else {
            out_stream.write(in2_data);
            in2_cnt++;
            if (in2_cnt < size)
                in2_data = in2_stream.read();
        }
    }
}

void combine(hls::stream<int>& in1_stream, hls::stream<int>& in2_stream, hls::stream<int>& out_stream, int size) {
    for (int i = 0; i < size; i++) {
        out_stream.write(in1_stream.read());
        out_stream.write(in2_stream.read());
    }
}

void remove_duplicates(hls::stream<int>& in_stream, hls::stream<int>& out_stream, int size) {
    int read_cnt = 1;
    int write_cnt = 1;
    int prev = in_stream.read();
    out_stream.write(prev);

    while (write_cnt < size) {
        if (read_cnt == size) {
            out_stream.write(0);
            write_cnt++;
        } else {
            int curr = in_stream.read();
            read_cnt++;
            if (curr != prev) {
                out_stream.write(curr);
                prev = curr;
                write_cnt++;
            }
        }
    }
}

void compute(hls::stream<int>& in1_stream, hls::stream<int>& in2_stream, hls::stream<int>& out_stream, int size) {
#pragma HLS dataflow
    hls::stream<int> compare_stream;
    compare1(in1_stream, in2_stream, compare_stream, size);
    remove_duplicates(compare_stream, out_stream, size * 2);
}

void load(int* in1, int* in2, hls::stream<int>& in1_stream, hls::stream<int>& in2_stream, int size) {
    for (int i = 0; i < size; i++) {
// #pragma HLS PIPELINE II=1
        in1_stream.write(in1[i]);
        in2_stream.write(in2[i]);
    }
}

void store(int* out, hls::stream<int>& out_stream, int size) {
    for (int i = 0; i < size * 2; i++) {
// #pragma HLS PIPELINE II=1
        out[i] = out_stream.read();
    }
}

extern "C" {

void merge(int* in1, int* in2, int* out, int size) {
#pragma HLS INTERFACE m_axi port = in1 bundle = gmem0 depth=4096
#pragma HLS INTERFACE m_axi port = in2 bundle = gmem1 depth=4096
#pragma HLS INTERFACE m_axi port = out bundle = gmem0 depth=4096

    hls::stream<int> in1_stream;
    hls::stream<int> in2_stream;
    hls::stream<int> out_stream;

// this pragma is a must, otherwise the kernel will not stop
#pragma HLS dataflow
    hls::print("start\n");
    load(in1, in2, in1_stream, in2_stream, size);
    compute(in1_stream, in2_stream, out_stream, size);
    store(out, out_stream, size);
    hls::print("finish\n");
}

} // extern C