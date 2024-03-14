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
#include <cstring>
#include <fcntl.h>

// XRT includes
#include "xrt/xrt_bo.h"
#include "xrt/xrt_device.h"
#include "xrt/xrt_kernel.h"

#define DATA_SIZE 4096*32
#define DATA_FILE "/tmp/data.bin"
#define RESULT_FILE "/tmp/result.bin"

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

    auto krnl = xrt::kernel(device, uuid, "adder");

    std::cout << "Allocate P2P Buffer in Global Memory\n";
    auto bo0 = xrt::bo(device, vector_size_bytes, xrt::bo::flags::p2p, krnl.group_id(0));
    auto bo1 = xrt::bo(device, vector_size_bytes, xrt::bo::flags::p2p, krnl.group_id(1));
    auto bo_out = xrt::bo(device, vector_size_bytes, krnl.group_id(2));

    // Map the contents of the buffer object into host memory
    auto bo0_map = bo0.map<int*>();
    auto bo1_map = bo1.map<int*>();
    auto bo_out_map = bo_out.map<int*>();

    // O_DIRECT requires 4K aligned buffer
    if (((uintptr_t)bo0_map % 4096) != 0)
        throw std::runtime_error("bo0_map is not aligned to 4KB");
    if (((uintptr_t)bo1_map % 4096) != 0)
        throw std::runtime_error("bo1_map is not aligned to 4KB");

    int fd = open(DATA_FILE, O_RDONLY | O_DIRECT);
    std::cout << "open data file " << DATA_FILE << ", fd: " << fd << std::endl;
    int ret = pread(fd, bo0_map, vector_size_bytes, 0);
    if (ret <= 0) {
        std::cerr << "ERR1: pread failed: " << ret
                  << " error: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    ret = pread(fd, bo1_map, vector_size_bytes, vector_size_bytes);
    if (ret <= 0) {
        std::cerr << "ERR2: pread failed: " << ret
                  << " error: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    close(fd);

    std::cout << "Execution of the kernel\n";
    auto run = krnl(bo0, bo1, bo_out, DATA_SIZE);
    run.wait();

    // Get the output;
    std::cout << "Get the output data from the device" << std::endl;
    bo_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);

    // Open the result file
    int result_fd = open(RESULT_FILE, O_RDONLY);
    int *bufReference = new int[DATA_SIZE];
    pread(result_fd, bufReference, vector_size_bytes, 0);
    close(result_fd);

    for (int i = 0; i < 10; i++) {
        std::cout << "Result[" << i << "] = " << bo_out_map[i] << " Reference[" << i << "] = " << bufReference[i] << std::endl;
    }

    // Validate our results
    if (std::memcmp(bo_out_map, bufReference, DATA_SIZE))
        throw std::runtime_error("Value read back does not match reference");

    std::cout << "TEST PASSED\n";
    return 0;
}