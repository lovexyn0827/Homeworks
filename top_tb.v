`timescale 1ns / 1ps

module top_tb;

reg CLK, RST;

top uut(
    .CLK(CLK), 
    .RST(RST)
);

initial begin
    CLK = 1;
    RST = 1;

    #20 RST = 1'b0;

    #200 $STOP;
end

always #5 CLK = ~CLK;

endmodule