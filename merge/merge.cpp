#include <ap_int.h>
#include <hls_stream.h>

typedef hls::stream<int, 256> stream;

void merge_stream(stream& in1_stream, stream& in2_stream, stream& out_stream, int size) {
    int in1_cnt = 0;
    int in2_cnt = 0;
    int in1_data = in1_stream.read();
    int in2_data = in2_stream.read();

    while (in1_cnt != size || in2_cnt != size) {
        if (in1_cnt != size && (in2_cnt == size || in1_data < in2_data)) {
            out_stream.write(in1_data);
            in1_cnt++;
            if (in1_cnt < size) {
                in1_data = in1_stream.read();
            }
        } else if (in2_cnt < size) {
            out_stream.write(in2_data);
            in2_cnt++;
            if (in2_cnt < size) {
                in2_data = in2_stream.read();
            }
        }
    }
}

void remove_duplicates(stream& in_stream, stream& out_stream, int size) {
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

void compute(stream& in1_stream, stream& in2_stream, stream& out_stream, int size) {
#pragma HLS dataflow
    stream tmp;
    merge_stream(in1_stream, in2_stream, tmp, size);
    remove_duplicates(tmp, out_stream, size * 2);
}

void load(int* in, stream& out_stream, int size) {
    for (int i = 0; i < size; i++) {
        out_stream.write(in[i]);
    }
}

void store(int* out, stream& out_stream, int size) {
    for (int i = 0; i < size; i++) {
        out[i] = out_stream.read();
    }
}

extern "C" {

void merge(int* in1, int* in2, int* out, int size) {
#pragma HLS INTERFACE m_axi port = in1 bundle = gmem0 depth=4096
#pragma HLS INTERFACE m_axi port = in2 bundle = gmem1 depth=4096
#pragma HLS INTERFACE m_axi port = out bundle = gmem0 depth=4096

    stream in1_stream;
    stream in2_stream;
    stream out_stream;

#pragma HLS dataflow
    load(in1, in1_stream, size);
    load(in2, in2_stream, size);
    compute(in1_stream, in2_stream, out_stream, size);
    store(out, out_stream, size * 2);
}

} // extern C