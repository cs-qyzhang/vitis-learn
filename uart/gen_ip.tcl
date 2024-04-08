# /*
# Copyright (C) 2023, Advanced Micro Devices, Inc. All rights reserved.
# SPDX-License-Identifier: X11
# */

set_part [lindex $argv 0]
set board [lindex $argv 1]

# ----------------------------------------------------------------------------
# generate Ethernet System IP of one channel
# ----------------------------------------------------------------------------
create_ip -name uart_ip \
          -vendor xilinx.com \
          -library ip \
          -version 1.* \
          -module_name uart_ip_0 \
          -dir ./ip_generation

# set_property -dict [list CONFIG.LINE_RATE {10} \
#                          CONFIG.BASE_R_KR {BASE-R} \
#                          CONFIG.INCLUDE_AXI4_INTERFACE {0} \
#                          CONFIG.ENABLE_PIPELINE_REG {1}] \
#              [get_ips xxv_ethernet_0]
