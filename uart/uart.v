`timescale 1 ns / 1 ps

module uart #(
    parameter integer C_S_AXI_CONTROL_ADDR_WIDTH = 12 ,
    parameter integer C_S_AXI_CONTROL_DATA_WIDTH = 32 ,
    parameter integer C_M_AXI_ADDR_WIDTH       = 64 ,
    parameter integer C_M_AXI_DATA_WIDTH       = 64,
    parameter integer C_M_AXI_ID_WIDTH         = 1,
    parameter integer C_XFER_SIZE_WIDTH        = 32,
    parameter integer C_M00_AXI_ADDR_WIDTH       = 64 ,
    parameter integer C_M00_AXI_DATA_WIDTH       = 512
)
(
    // clock and reset
    input  wire                                     ap_clk,
    input  wire                                     ap_rst_n             ,
    // AXI4-Lite interface, s_axi_control
    input  wire                                     s_axi_control_awvalid,
    output wire                                     s_axi_control_awready,
    input  wire [C_S_AXI_CONTROL_ADDR_WIDTH-1:0]    s_axi_control_awaddr ,
    input  wire                                     s_axi_control_wvalid ,
    output wire                                     s_axi_control_wready ,
    input  wire [C_S_AXI_CONTROL_DATA_WIDTH-1:0]    s_axi_control_wdata  ,
    input  wire [C_S_AXI_CONTROL_DATA_WIDTH/8-1:0]  s_axi_control_wstrb  ,
    input  wire                                     s_axi_control_arvalid,
    output wire                                     s_axi_control_arready,
    input  wire [C_S_AXI_CONTROL_ADDR_WIDTH-1:0]    s_axi_control_araddr ,
    output wire                                     s_axi_control_rvalid ,
    input  wire                                     s_axi_control_rready ,
    output wire [C_S_AXI_CONTROL_DATA_WIDTH-1:0]    s_axi_control_rdata  ,
    output wire [2-1:0]                             s_axi_control_rresp  ,
    output wire                                     s_axi_control_bvalid ,
    input  wire                                     s_axi_control_bready ,
    output wire [2-1:0]                             s_axi_control_bresp  ,
    // AXI4 master interface m00_axi
    output wire                                     m00_axi_awvalid      ,
    input  wire                                     m00_axi_awready      ,
    output wire [C_M00_AXI_ADDR_WIDTH-1:0]          m00_axi_awaddr       ,
    output wire [8-1:0]                             m00_axi_awlen        ,
    output wire                                     m00_axi_wvalid       ,
    input  wire                                     m00_axi_wready       ,
    output wire [C_M00_AXI_DATA_WIDTH-1:0]          m00_axi_wdata        ,
    output wire [C_M00_AXI_DATA_WIDTH/8-1:0]        m00_axi_wstrb        ,
    output wire                                     m00_axi_wlast        ,
    input  wire                                     m00_axi_bvalid       ,
    output wire                                     m00_axi_bready       ,
    output wire                                     m00_axi_arvalid      ,
    input  wire                                     m00_axi_arready      ,
    output wire [C_M00_AXI_ADDR_WIDTH-1:0]          m00_axi_araddr       ,
    output wire [8-1:0]                             m00_axi_arlen        ,
    input  wire                                     m00_axi_rvalid       ,
    output wire                                     m00_axi_rready       ,
    input  wire [C_M00_AXI_DATA_WIDTH-1:0]          m00_axi_rdata        ,
    input  wire                                     m00_axi_rlast        ,
    // uart
    input  wire                                     uart_rx,
    output wire                                     uart_tx
);

assign uart_tx = uart_rx;

endmodule