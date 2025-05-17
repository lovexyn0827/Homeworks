/* verilator lint_off DECLFILENAME */

module RF (
    input wire[4:0] RI1, RI2, WI, 
    input wire[31:0] WD, 
    input wire CLK, RST, WRT, 
    output wire[31:0] RD1, RD2
);

reg[31:0] regs[31:0];

assign RD1 = regs[RI1];
assign RD2 = regs[RI2];

always @(posedge CLK) begin
    if (RST)
        for (integer i = 0; i < 32; i = i + 1)
            regs[i] <= 0;
    else if (WRT)
        regs[WI] <= WD;
end

endmodule

module ALU (
    input wire[31:0] A, B, 
    input wire[3:0] FUNC, 
    output wire[31:0] OUT, 
    output wire /*LT, EQ, GT*/ EQ
);

wire[31:0] outs[15:0];

assign outs[0] = A + B;
assign outs[1] = A - B;
assign outs[2] = A * B;
assign outs[3] = A / B;
assign outs[4] = A % B;
assign outs[5] = A << B;
assign outs[6] = A >> B;

assign outs[7] = A >>> B;
assign outs[8] = A ^ B;
assign outs[9] = A & B;
assign outs[10] = A | B;
assign outs[11] = ~A;
assign outs[12] = 0;
assign outs[13] = 0;
assign outs[14] = 0;
assign outs[15] = 0;
assign OUT = outs[FUNC];

//assign LT = OUT[31];
assign EQ = OUT == 0;
//assign GT = !(LT ^ GT);

endmodule

module IR (
    input wire CLK, RST, 
    input wire[31:0] INSN, 
    output wire[5:0] OPC, FUNC, 
    output wire[25:0] JADDR, 
    output wire[15:0] IMM, 
    output wire[4:0] RS, RT, RD/*, SHAMT*/
);

reg[31:0] IR;
wire[31:0] EffIR;
assign EffIR = RST ? 32'h08000000 : IR;

always @(posedge CLK) begin
    IR <= INSN;
end

assign OPC = EffIR[31:26];
assign FUNC = EffIR[5:0];

assign JADDR = EffIR[25:0];
assign IMM = EffIR[15:0];

assign RS = EffIR[25:21];
assign RT = EffIR[20:16];
assign RD = EffIR[15:11];
//assign SHAMT = EffIR[10:6];

endmodule

module CU (
    input wire[5:0] OPC, FUNC, 
    input wire /*LT, GT,*/ EQ, 
    output wire[3:0] ALUC, 
    output wire RDST, ALUB, M2R, RWRT, MWRT, VLD, EXOPS, 
    output wire[1:0] NPC
);

wire RType = OPC == 6'b0;
wire ADD = RType & FUNC == 6'b100000;
wire SUB = RType & FUNC == 6'b100010;
wire AND = RType & FUNC == 6'b100100;
wire OR = RType & FUNC == 6'b100101;
wire J = OPC == 6'b000010;
wire BEQ = OPC == 6'b000100;
wire ADDI = OPC == 6'b001000;
wire LW = OPC == 6'b100011;
wire SW = OPC == 6'b101011;

wire[1:0] NPCHard = J ? 2'b10 : (BEQ ? 2'b01 : 2'b00);

assign RDST = RType;
assign ALUB = ADDI | LW | SW;
assign M2R = LW;
assign MWRT = SW;
assign RWRT = RType | ADDI | LW;
assign VLD = ADD | SUB | AND | OR | J | BEQ | ADDI | LW | SW;
assign EXOPS = ADDI | BEQ | LW | SW;

assign NPC = NPCHard == 2'b01 ? (EQ ? 2'b01 : 2'b00) : NPCHard;

assign ALUC[0] = SUB | OR | BEQ;
assign ALUC[1] = AND | OR;
assign ALUC[3:2] = 2'b00;

endmodule

module PC (
    input wire[25:0] JADDR, 
    input wire[31:0] OFFSET, 
    input wire[1:0] NPC, 
    input wire CLK, RST, 
    output wire[31:0] PC
);

reg[31:0] PCReg;

wire[31:0] PCP4 = PCReg + 4;
wire[31:0] NPCVal = NPC[1] ? ({ PCP4[31:28], JADDR, 2'b00 }) : (PCP4 + (NPC[0] ? OFFSET << 2 : 32'b0));
assign PC = NPCVal;

always @(posedge CLK) begin
    PCReg <= RST ? 32'h00000000 : NPCVal;
end

endmodule

module CPU (
    input wire[31:0] INSN, DATIN, 
    input wire CLK, RST, 
    output wire[31:0] DATOUT, DATADDR, INSNADDR, 
    output wire MWRT
);

wire /*LT, GT,*/ EQ;
wire[5:0] Opc, Func;
wire[25:0] JAddr; 
wire[15:0] ImmUnExted; 
wire[31:0] Imm;
wire[4:0] RS, RT, RD/*, Shamt*/;

IR ir(
    .INSN(INSN), 
	.CLK(EffClk), 
    .RST(RST), 
    .OPC(Opc), 
    .FUNC(Func), 
    .JADDR(JAddr), 
    .IMM(ImmUnExted), 
    .RS(RS), 
    .RT(RT), 
    .RD(RD)
    //.SHAMT(Shamt)
);

wire[3:0] AluC;
wire[1:0] NPC;

wire RDst, AluB, M2R, RWrt,  ExOps, Vld;

CU cu(
    .OPC(Opc), 
    .FUNC(Func), 
    //.LT(LT), 
    .EQ(EQ), 
    //.GT(GT), 
    .ALUC(AluC), 
    .RDST(RDst), 
    .ALUB(AluB), 
    .M2R(M2R), 
    .RWRT(RWrt), 
    .MWRT(MWRT), 
    .NPC(NPC), 
	.EXOPS(ExOps), 
    .VLD(Vld)
);

assign Imm = ExOps ? { { 16{ ImmUnExted[15] } }, ImmUnExted } : { 16'b0, ImmUnExted };

wire EffClk;
assign EffClk = Vld & CLK;

wire[31:0] A, AluOut, RFOut2;

RF rf(
    .RI1(RS), 
    .RI2(RT), 
    .WI(RDst ? RD : RT), 
    .CLK(EffClk), 
    .RST(RST), 
    .WRT(RWrt), 
    .RD1(A), 
    .RD2(RFOut2), 
    .WD(M2R ? DATIN : AluOut)
);

ALU alu(
    .A(A), 
    .B(AluB ? Imm : RFOut2), 
    .FUNC(AluC), 
    .OUT(AluOut), 
	//.LT(LT), 
	//.GT(GT), 
	.EQ(EQ)
);

assign DATADDR = AluOut;

PC pc(
    .JADDR(JAddr), 
    .OFFSET(Imm), 
    .NPC(NPC), 
    .CLK(EffClk), 
    .RST(RST), 
    .PC(INSNADDR)
);

assign DATOUT = MWRT ? RFOut2 : 32'hZZZZZZZZ; 

endmodule

module ROM (
	input wire[31:0] ADDR, 
	output reg[31:0] DAT
);

always @(ADDR) begin
    case ((ADDR & 32'hff) >> 2)
        32'h00000000: DAT <= 32'h20100000;
        32'h00000001: DAT <= 32'h20110000;
        32'h00000002: DAT <= 32'h00009024;
        32'h00000003: DAT <= 32'h2008000a;
        32'h00000004: DAT <= 32'h11000001;
        32'h00000005: DAT <= 32'h08100007;
        32'h00000006: DAT <= 32'h08100006;
        32'h00000007: DAT <= 32'h12280004;
        32'h00000008: DAT <= 32'h02129020;
        32'h00000009: DAT <= 32'h22100001;
        32'h0000000A: DAT <= 32'h22310001;
        32'h0000000B: DAT <= 32'h08100007;
        32'h0000000C: DAT <= 32'hac120040;
        32'h0000000D: DAT <= 32'h8c100040;
        32'h0000000E: DAT <= 32'h0800000e;
        default: DAT <= 32'h00000000;
    endcase
end

endmodule

module RAM (
	input wire[31:0] ADDR, 
	input wire MWRT, CLK, RST, 
	inout wire[31:0] DAT
);

reg[31:0] mem[255:0];

always @(posedge CLK) begin
    if (RST)
        for (integer i = 0; i < 256; i = i + 1)
            mem[i] <= 0;
    else if (MWRT)
        mem[ADDR[9:2]] <= DAT;
end

assign DAT = MWRT ? 32'hZZZZZZZZ : (ADDR > 32'hFF ? 32'h0 : mem[ADDR[9:2]]);

endmodule

module top(
	input wire CLK, RST
);

wire MWrt;
wire[31:0] Insn, DatBus;
wire[31:0] DatAddr, InsnAddr;

ROM progMem(
	.ADDR(InsnAddr), 
	.DAT(Insn)
);

RAM dataMem(
	.ADDR(DatAddr), 
	.DAT(DatBus), 
	.MWRT(MWrt), 
	.CLK(CLK), 
    .RST(RST)
);

CPU cpu(
    .DATIN(DatBus), 
    .INSN(Insn), 
    .DATADDR(DatAddr), 
    .INSNADDR(InsnAddr), 
    .CLK(CLK), 
    .RST(RST), 
    .DATOUT(DatBus), 
    .MWRT(MWrt)
);

endmodule
