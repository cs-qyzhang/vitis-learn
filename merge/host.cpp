/**
* Copyright (C) 2019-2021 Xilinx, Inc
*
* Licensed under the Apache License, Version 2.0 (the "License"). You may
* not use this file except in compliance with the License. A copy of the
* License is located at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
* License for the specific language governing permissions and limitations
* under the License.
*/

#include <iostream>
#include <string>
#include <unistd.h>
#include <random>
#include <cstring>

// XRT includes
#include "xrt/xrt_bo.h"
#include "xrt/xrt_device.h"
#include "xrt/xrt_kernel.h"

#define DATA_SIZE 4096*1024

void generate_random(int *vec, int size) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 1000000);
    for (int i = 0; i < size; i++) {
        vec[i] = dis(gen);
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <xclbin>" << std::endl;
        return EXIT_FAILURE;
    }

    // Read settings
    std::string binaryFile = argv[1];
    int device_index = 0;

    std::cout << "Open the device" << device_index << std::endl;
    auto device = xrt::device(device_index);
    auto device_name = device.get_info<xrt::info::device::name>();

    std::cout << "Load the xclbin " << binaryFile << std::endl;
    auto uuid = device.load_xclbin(binaryFile);

    size_t vector_size_bytes = sizeof(int) * DATA_SIZE;

    auto krnl = xrt::kernel(device, uuid, "merge");

    std::cout << "Allocate Buffer in Global Memory\n";
    auto bo0 = xrt::bo(device, vector_size_bytes, krnl.group_id(0));
    auto bo1 = xrt::bo(device, vector_size_bytes, krnl.group_id(1));
    auto bo_out = xrt::bo(device, vector_size_bytes * 2, krnl.group_id(2));

    // Map the contents of the buffer object into host memory
    auto bo0_map = bo0.map<int*>();
    auto bo1_map = bo1.map<int*>();
    auto bo_out_map = bo_out.map<int*>();
    // std::fill(bo0_map, bo0_map + DATA_SIZE, 0);
    // std::fill(bo1_map, bo1_map + DATA_SIZE, 0);
    std::fill(bo_out_map, bo_out_map + DATA_SIZE * 2, 0);

    // Create the test data
    int *bufReference = new int[DATA_SIZE * 2];
    generate_random(bo0_map, DATA_SIZE);
    std::sort(bo0_map, bo0_map + DATA_SIZE);
    generate_random(bo1_map, DATA_SIZE);
    std::sort(bo1_map, bo1_map + DATA_SIZE);

    // test duplication case
    // memcpy(bo1_map, bo0_map, vector_size_bytes);

    // sort data for test
    int *tmp = new int[DATA_SIZE * 2];
    memcpy(tmp, bo0_map, vector_size_bytes);
    memcpy(tmp + DATA_SIZE, bo1_map, vector_size_bytes);
    std::sort(tmp, tmp + DATA_SIZE * 2);

    // remove duplicates
    std::fill(bufReference, bufReference + DATA_SIZE * 2, 0);
    int j = 0;
    int prev = tmp[DATA_SIZE * 2 - 1];
    for (int i = 0; i < DATA_SIZE * 2; i++) {
        if (tmp[i] != prev) {
            bufReference[j++] = tmp[i];
        }
        prev = tmp[i];
    }

    // Synchronize buffer content with device side
    std::cout << "synchronize input buffer data to device global memory\n";

    bo0.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    bo1.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    std::cout << "Execution of the kernel\n";
    auto run = krnl(bo0, bo1, bo_out, DATA_SIZE);
    run.wait();

    // Get the output;
    std::cout << "Get the output data from the device" << std::endl;
    bo_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);

    // Validate our results
    if (std::memcmp(bo_out_map, bufReference, DATA_SIZE * 2))
        throw std::runtime_error("Value read back does not match reference");

    std::cout << "TEST PASSED\n";
    return 0;
}