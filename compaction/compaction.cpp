#include <ap_int.h>
#include <hls_stream.h>

typedef hls::stream<int, 256> stream;

// index block
void min_max(stream& key_in, bool block_finish, stream& out) {
    int min = in.read();
    int max = min;

    // global memory init

    while (true) {
        int data = in.read();
        if (data < min) {
            min = data;
        } else if (data > max) {
            max = data;
        }
        if (block_finish) {
            out.write(min);
            out.write(max);
        }
    }

    out.write(min);
    out.write(max);

    // global memory write
}

// data block
void merge(stream& in1, stream& in2, stream& key_out, bool block_finish, stream& data_block, stream& restart, size_t size, bool finish) {
    int bytes_read = 0;

    finish = false;

    while (true) {
        block_finish = false;
        if (bytes_read == size)
            return;
        // parse in1
        shared_len = in1.read();
        non_shared_len = in1.read();
        value_len = in1.read();
        non_shared_Key = in1.read();
        value = in1.read();

        char key1[1024];
        shared + non_shared -> key1

        // parse in2
        ...

        // merge
        if (key1 < key2) {
            key_out.write(key1);
            data_block.write(shared_len);
            ...
        }
        if (bytes_read == block_size)
            block_finish = true;
    }

    finish = true;
}

void bloom(stream& key_in, bool block_finish, stream& out, stream& out_global, bool finish) {
    uint32_t hash_value[4096];
    int counter = 0;

    // init
    global memory -> hash_value
    global memory -> counter

    while (!finish) {
        if (!block_finish) {
            int key = key_in.read();
            for (int i = 0; i < 4096; i++) {
                hash_value[i] = hash(key, i);
            }
            counter++;
        } else {
            // 1. generate filter block
            ...
            counter = 0;
        }
    }

    hash_value -> global memory
    counter -> global memory
}

void crc(stream& in, bool block_finish, stream& out) {
    // global memory init

    // global memory dump
}

void merge_stream(stream& in1_stream, stream& in2_stream, int* restart_stream, stream& out_stream, int size) {
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