# /*
# See Vitis-Tutorials/Hardware_Acceleration/Feature_Tutorials/09-using-ethernet-on-alveo/pack_eth_x1_kernel.tcl
# See U55C board file board.xml
# */

## Create ip project with part name in command line argvs
create_project uart_kernel ./uart_kernel -part [lindex $argv 0]

## Add design sources into project
add_files -norecurse    \
    {                   \
        ../uart.v        \
    }

update_compile_order -fileset sources_1

## Create IP packaging project
ipx::package_project -root_dir ./kernel_pack -vendor jianyue.tech -library csd -taxonomy /UserIP -import_files -set_current true

## Set uart interface
ipx::add_bus_interface uart_port [ipx::current_core]
# master, see board file
set_property interface_mode master [ipx::get_bus_interfaces uart_port -of_objects [ipx::current_core]]
# xilinx.com:interface:uart_rtl:1.0, see board file
set_property abstraction_type_vlnv xilinx.com:interface:uart_rtl:1.0 [ipx::get_bus_interfaces uart_port -of_objects [ipx::current_core]]
set_property bus_type_vlnv xilinx.com:interface:uart:1.0 [ipx::get_bus_interfaces uart_port -of_objects [ipx::current_core]]
# TxD, see board file
ipx::add_port_map TxD [ipx::get_bus_interfaces uart_port -of_objects [ipx::current_core]]
# uart_tx should be the signal in the RTL
set_property physical_name uart_tx [ipx::get_port_maps TxD -of_objects [ipx::get_bus_interfaces uart_port -of_objects [ipx::current_core]]]
ipx::add_port_map RxD [ipx::get_bus_interfaces uart_port -of_objects [ipx::current_core]]
set_property physical_name uart_rx [ipx::get_port_maps RxD -of_objects [ipx::get_bus_interfaces uart_port -of_objects [ipx::current_core]]]

ipx::associate_bus_interfaces -busif s_axi_control -clock ap_clk [ipx::current_core]
ipx::associate_bus_interfaces -busif m00_axi -clock ap_clk [ipx::current_core]

set_property xpm_libraries {XPM_CDC XPM_MEMORY XPM_FIFO} [ipx::current_core]
set_property supported_families { } [ipx::current_core]
set_property auto_family_support_level level_2 [ipx::current_core]
ipx::update_checksums [ipx::current_core]

## Set required property for Vitis kernel
set_property sdx_kernel true [ipx::current_core]
set_property sdx_kernel_type rtl [ipx::current_core]

## Packaging Vivado IP
ipx::update_source_project_archive -component [ipx::current_core]
ipx::save_core [ipx::current_core]

# Generate Vitis Kernel from Vivado IP
package_xo -force -xo_path ./uart.xo \
           -kernel_name uart \
           -ctrl_protocol ap_ctrl_hs \
           -ip_directory ./kernel_pack \
           -kernel_xml ../kernel_xo.xml
        #    -output_kernel_xml ./uart.xml
