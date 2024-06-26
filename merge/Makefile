#
# Copyright 2019-2021 Xilinx, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# makefile-generator v1.0.3
#
TARGET := hw
PLATFORM := xilinx_u55c_gen3x16_xdma_3_202210_1
EXECUTABLE = ./merge_host
KERNEL = merge
HOST_FILE = host.cpp
VITIS_CONFIG = ./vitis.cfg
RUN_ARGS ?=
DEBUG := no

############################## Help Section ##############################
help:
	echo "Makefile Usage:"
	echo "  make all TARGET=<sw_emu/hw_emu/hw> PLATFORM=<FPGA platform>"
	echo "      Command to generate the design for specified Target and Shell."
	echo ""
	echo "  make run TARGET=<sw_emu/hw_emu/hw> PLATFORM=<FPGA platform>"
	echo "      Command to run application in emulation."
	echo ""
	echo "  make build TARGET=<sw_emu/hw_emu/hw> PLATFORM=<FPGA platform>"
	echo "      Command to build xclbin application."
	echo ""
	echo "  make host"
	echo "      Command to build host application."
	echo ""
	echo "  make clean "
	echo "      Command to remove the generated non-hardware files."
	echo ""
	echo "  make cleanall"
	echo "      Command to remove all the generated files."
	echo ""

############################## Setting up Project Variables ##############################
VPP_LDFLAGS := --vivado.prop run.impl_1.strategy=Flow_RuntimeOptimized
#Generates debug summary report
ifeq ($(DEBUG), yes)
VPP_LDFLAGS += --dk list_ports
endif

TEMP_DIR := ./_x.$(TARGET)
BUILD_DIR := ./build_dir.$(TARGET)

LINK_OUTPUT := $(BUILD_DIR)/$(KERNEL).link.xclbin
PACKAGE_OUT = ./package.$(TARGET)

VPP_PFLAGS := 
CMD_ARGS = $(BUILD_DIR)/$(KERNEL).xclbin $(RUN_ARGS)

CXXFLAGS += -I$(XILINX_XRT)/include -I$(XILINX_VIVADO)/include -Wall -O0 -g -std=c++1y
LDFLAGS += -L$(XILINX_XRT)/lib -pthread -lOpenCL

############################## Setting up Host Variables ##############################
#Include Required Host Source Files
HOST_SRCS += $(HOST_FILE) 
# Host compiler global settings
CXXFLAGS += -fmessage-length=0
LDFLAGS += -lrt -lstdc++ 
LDFLAGS += -luuid -lxrt_coreutil

############################## Setting up Kernel Variables ##############################
# Kernel compiler global settings
VPP_FLAGS += --save-temps --config $(VITIS_CONFIG)
#--optimize quick
#--vivado.impl.jobs 32 --vivado.synth.jobs 32 --hls.jobs 32
ifneq ($(TARGET), hw)
VPP_FLAGS += -g
endif

EMCONFIG_DIR = $(TEMP_DIR)

############################## Setting Targets ##############################
.PHONY: all clean cleanall emconfig
all: $(EXECUTABLE) $(BUILD_DIR)/$(KERNEL).xclbin emconfig

.PHONY: host
host: $(EXECUTABLE)

.PHONY: build
build: $(BUILD_DIR)/$(KERNEL).xclbin

.PHONY: xclbin
xclbin: build

############################## Setting Rules for Binary Containers (Building Kernels) ##############################
$(TEMP_DIR)/$(KERNEL).xo: $(KERNEL).cpp $(VITIS_CONFIG)
	mkdir -p $(TEMP_DIR)
	v++ -c $(VPP_FLAGS) -t $(TARGET) --platform $(PLATFORM) -k $(KERNEL) --temp_dir $(TEMP_DIR)  -I'$(<D)' -o'$@' '$<'

$(BUILD_DIR)/$(KERNEL).xclbin: $(TEMP_DIR)/$(KERNEL).xo $(VITIS_CONFIG)
	mkdir -p $(BUILD_DIR)
	v++ -l $(VPP_FLAGS) $(VPP_LDFLAGS) -t $(TARGET) --platform $(PLATFORM) --temp_dir $(TEMP_DIR) -o'$(LINK_OUTPUT)' $<
	v++ -p $(LINK_OUTPUT) $(VPP_FLAGS) -t $(TARGET) --platform $(PLATFORM) --package.out_dir $(PACKAGE_OUT) -o $(BUILD_DIR)/$(KERNEL).xclbin

############################## Setting Rules for Host (Building Host Executable) ##############################
$(EXECUTABLE): $(HOST_SRCS)
		g++ -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

emconfig:$(EMCONFIG_DIR)/emconfig.json
$(EMCONFIG_DIR)/emconfig.json:
	emconfigutil --platform $(PLATFORM) --od $(EMCONFIG_DIR)

############################## Setting Essential Checks and Running Rules ##############################
run: all
ifeq ($(TARGET),$(filter $(TARGET),sw_emu hw_emu))
	cp -rf $(EMCONFIG_DIR)/emconfig.json .
	XCL_EMULATION_MODE=$(TARGET) $(EXECUTABLE) $(CMD_ARGS)
else
	$(EXECUTABLE) $(CMD_ARGS)
endif

.PHONY: test
test: $(EXECUTABLE)
ifeq ($(TARGET),$(filter $(TARGET),sw_emu hw_emu))
	XCL_EMULATION_MODE=$(TARGET) $(EXECUTABLE) $(CMD_ARGS)
else
	$(EXECUTABLE) $(CMD_ARGS)
endif

############################## Cleaning Rules ##############################
# Cleaning stuff
clean:
	-rm -rf $(EXECUTABLE) $(XCLBIN)/{*sw_emu*,*hw_emu*} 
	-rm -rf profile_* TempConfig system_estimate.xtxt *.rpt *.csv 
	-rm -rf *.ll *v++* .Xil emconfig.json dltmp* xmltmp* *.log *.jou *.wcfg *.wdb

cleanall: clean
	-rm -rf build_dir*
	-rm -rf package.*
	-rm -rf _x* *xclbin.run_summary qemu-memory-_* emulation _vimage pl* start_simulation.sh *.xclbin
