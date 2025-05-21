/* verilator lint_off DECLFILENAME */

module RF (
    input wire[4:0] RI1, RI2, WI, 
    input wire[31:0] WD, 
    input wire CLK, RST, WRT, 
    output wire[31:0] RD1, RD2
);

reg[31:0] regs[31:1];

assign RD1 = regs[RI1];
assign RD2 = regs[RI2];

always @(posedge CLK) begin
    if (RST)
        for (integer i = 0; i < 32; i = i + 1)
            regs[i] <= 0;
    else if (WRT & WI != 5'b0)
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
assign outs[2] = A & B;
assign outs[3] = A | B;
assign outs[4] = A % B;
assign outs[5] = A << B;
assign outs[6] = A >> B;

assign outs[7] = A >>> B;
assign outs[8] = A ^ B;
assign outs[9] = A * B;
assign outs[10] = A / B;
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

module InsnTokenizer (
    input wire[31:0] INSN, 
    output wire[5:0] OPC, FUNC, 
    output wire[25:0] JADDR, 
    output wire[15:0] IMM, 
    output wire[4:0] RS, RT, RD/*, SHAMT*/
);

assign OPC = INSN[31:26];
assign FUNC = INSN[5:0];

assign JADDR = INSN[25:0];
assign IMM = INSN[15:0];

assign RS = INSN[25:21];
assign RT = INSN[20:16];
assign RD = INSN[15:11];
//assign SHAMT = INSN[10:6];

endmodule

module CU (
    input wire[5:0] OPC, FUNC, 
    //input wire /*LT, GT,*/ EQ, 
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

assign NPC = NPCHard;// == 2'b01 ? (EQ ? 2'b01 : 2'b00) : NPCHard;

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

// IF => ID => EX => MEM => WB

module SegRegsIF2ID(
    input wire CLK, RST, WRT, NOP, 
    input wire[31:0] INSN_F, 
    input wire[31:0] PCP4_F, 
    output reg[31:0] INSN_D, 
    output reg[31:0] PCP4_D
);

always @(posedge CLK) begin
    if (WRT) begin
        INSN_D <= NOP ? 32'h00000020 : (RST ? 32'b0 : INSN_F);
        PCP4_D <= RST ? 32'b0 : PCP4_F;
    end
end

endmodule

module SegRegsID2EXE(
    input wire CLK, RST, WRT, 
    input wire[3:0] ALUC_D, 
    input wire ALUB_D, M2R_D, RWRT_D, MWRT_D, BREN_D, 
    input wire[31:0] QA_D, QB_D, PCP4_D, IMM_D, 
    input wire[4:0] WIDX_D, QAIDX_D, QBIDX_D, 
    output reg[3:0] ALUC_E, 
    output reg ALUB_E, M2R_E, RWRT_E, MWRT_E, BREN_E, 
    output reg[31:0] QA_E, QB_E, PCP4_E, IMM_E, 
    output reg[4:0] WIDX_E, QAIDX_E, QBIDX_E
);

always @(posedge CLK) begin
    if (WRT) begin
        ALUC_E <= RST ? 4'b0 : ALUC_D;
        ALUB_E <= RST ? 1'b0 : ALUB_D;
        M2R_E <= RST ? 1'b0 : M2R_D;
        RWRT_E <= RST ? 1'b0 : RWRT_D;
        MWRT_E <= RST ? 1'b0 : MWRT_D;
        BREN_E <= RST ? 1'b0 : BREN_D;
        QA_E <= RST ? 32'b0 : QA_D;
        QB_E <= RST ? 32'b0 : QB_D;
        PCP4_E <= RST ? 32'b0 : PCP4_D;
        IMM_E <= RST ? 32'b0 : IMM_D;
        WIDX_E <= RST ? 5'b0 : WIDX_D;
        QAIDX_E <= RST ? 5'b0 : QAIDX_D;
        QBIDX_E <= RST ? 5'b0 : QBIDX_D;
    end
end

endmodule

module SegRegsEXE2MEM(
    input wire CLK, RST, WRT, 
    input wire RWRT_E, MWRT_E, M2R_E, BRTKN_E, 
    input wire[4:0] WIDX_E, QAIDX_E, QBIDX_E, 
    input wire[31:0] ALUOUT_E, QB_E, PCBR_E, 
    output reg RWRT_M, MWRT_M, M2R_M, BRTKN_M, 
    output reg[4:0] WIDX_M, QAIDX_M, QBIDX_M, 
    output reg[31:0] ALUOUT_M, QB_M, PCBR_M
);

always @(posedge CLK) begin
    if (WRT) begin
        RWRT_M <= RST ? 1'b0 : RWRT_E;
        MWRT_M <= RST ? 1'b0 : MWRT_E;
        M2R_M <= RST ? 1'b0 : M2R_E;
        BRTKN_M <= RST ? 1'b0 : BRTKN_E;
        WIDX_M <= RST ? 5'b0 : WIDX_E;
        QAIDX_M <= RST ? 5'b0 : QAIDX_E;
        QBIDX_M <= RST ? 5'b0 : QBIDX_E;
        ALUOUT_M <= RST ? 32'b0 : ALUOUT_E;
        QB_M <= RST ? 32'b0 : QB_E;
        PCBR_M <= RST ? 32'b0 : PCBR_E;
    end
end

endmodule

module SegRegsMEM2WB(
    input wire CLK, RST, WRT, 
    input wire RWRT_M, 
    input wire[4:0] WIDX_M, 
    input wire[31:0] WDAT_M, 
    output reg RWRT_W, 
    output reg[4:0] WIDX_W, 
    output reg[31:0] WDAT_W 
);

always @(posedge CLK) begin
    if (WRT) begin
        RWRT_W <= RST ? 1'b0 : RWRT_M;
        WIDX_W <= RST ? 5'b0 : WIDX_M;
        WDAT_W <= RST ? 32'b0 : WDAT_M;
    end
end

endmodule

module CPU (
    input wire[31:0] INSN, DATIN, 
    input wire CLK, RST, 
    output wire[31:0] DATOUT, DATADDR, INSNADDR, 
    output wire MWRT
);

// Globals

wire EffClk;
wire SegRegsRstF2D, SegRegsRstD2E, SegRegsRstE2M, SegRegsRstM2W;
wire SegRegsWrtF2D, SegRegsWrtD2E, SegRegsWrtE2M, SegRegsWrtM2W;

assign SegRegsRstF2D = 1'b0;
assign SegRegsRstD2E = 1'b0;
assign SegRegsRstE2M = 1'b0;
assign SegRegsRstM2W = 1'b0;

assign SegRegsWrtM2W = 1'b1;

// Bypasses

wire[31:0] QA_Bypass, QB_Bypass, Mem_Bypass;
wire BypassQA, BypassQB, BypassMem;

// Stage I: Instruction Fetch

wire[31:0] PCP4_F, PCJ_F, PCBR_F;
wire BranchTaken_F, Jump_F;

reg[31:0] PC;

assign PCP4_F = PC + 4;
always @(posedge EffClk) begin
    PC <= RST ? 32'h00000000 : (Jump_F ? PCJ_F : (BranchTaken_F ? PCBR_F : PCP4_F));
end

assign INSNADDR = PC;
assign SegRegsWrtF2D = ~BranchTaken_F;
assign SegRegsWrtD2E = ~BranchTaken_F;
assign SegRegsWrtE2M = ~BranchTaken_F;

// Stage II: Instruction Decode

wire[31:0] PCP4_D, Insn_D;

SegRegsIF2ID IF2ID(
    .CLK(CLK), 
    .RST(RST | SegRegsRstF2D), 
    .WRT(SegRegsWrtF2D), 
    .NOP(Jump_F | BranchTaken_F), 
    .PCP4_F(PCP4_F), 
    .INSN_F(INSN),
    .PCP4_D(PCP4_D), 
    .INSN_D(Insn_D)
);

wire[5:0] Opc_D, Func_D;
wire[25:0] JAddr_D; 
wire[15:0] ImmUnExted_D; 
wire[31:0] Imm_D;
wire[4:0] RS_D, RT_D, RD_D/*, Shamt_D*/;

InsnTokenizer it(
    .INSN(Insn_D),  
    .OPC(Opc_D), 
    .FUNC(Func_D), 
    .JADDR(JAddr_D), 
    .IMM(ImmUnExted_D), 
    .RS(RS_D), 
    .RT(RT_D), 
    .RD(RD_D)
    //.SHAMT(Shamt)
);

wire[3:0] AluC_D;
wire[1:0] NPC_D;

wire RDst_D, AluB_D, M2R_D, RWrt_D, MWrt_D, ExOps_D, Vld_D;

CU cu(
    .OPC(Opc_D), 
    .FUNC(Func_D), 
    //.LT(LT), 
    //.EQ(EQ_D), 
    //.GT(GT), 
    .ALUC(AluC_D), 
    .RDST(RDst_D), 
    .ALUB(AluB_D), 
    .M2R(M2R_D), 
    .RWRT(RWrt_D), 
    .MWRT(MWrt_D), 
    .NPC(NPC_D), 
	.EXOPS(ExOps_D), 
    .VLD(Vld_D)
);

wire BrEn_D, Jump_D;
assign BrEn_D = NPC_D[0];
assign Jump_D = NPC_D[1];
assign Jump_F = Jump_D;
assign PCJ_F = { PCP4_D[31:28], JAddr_D, 2'b00 };

assign Imm_D = ExOps_D ? { { 16{ ImmUnExted_D[15] } }, ImmUnExted_D } : { 16'b0, ImmUnExted_D };

assign EffClk = Vld_D & CLK;

wire RWrt_W;
wire[31:0] QA_D, QB_D, WDat_D;
wire[4:0] WIdx_D, WIdx_W;
assign WIdx_D = RDst_D ? RD_D : RT_D;

RF rf(
    .CLK(EffClk), 
    .RST(RST), 
    .RI1(RS_D), 
    .RI2(RT_D), 
    .WI(WIdx_W), 
    .WRT(RWrt_W), 
    .RD1(QA_D), 
    .RD2(QB_D), 
    .WD(WDat_D)
);

assign QA_Bypass = WDat_M;
assign QB_Bypass = WDat_M;

// Stage III: Execution

wire[3:0] AluC_E;
wire AluB_E, M2R_E, RWrt_E, MWrt_E, BrEn_E;
wire[31:0] QA_E, QB_E, PCP4_E, Imm_E;
wire[4:0] WIdx_E, QAIdx_E, QBIdx_E;

SegRegsID2EXE ID2EXE(
    .CLK(EffClk), 
    .RST(RST | SegRegsRstD2E), 
    .WRT(SegRegsWrtD2E), 
    .ALUC_D(AluC_D), 
    .ALUB_D(AluB_D), 
    .M2R_D(M2R_D), 
    .RWRT_D(RWrt_D), 
    .MWRT_D(MWrt_D), 
    .BREN_D(BrEn_D), 
    .QA_D(QA_D), 
    .QB_D(QB_D),  
    .PCP4_D(PCP4_D), 
    .IMM_D(Imm_D), 
    .WIDX_D(WIdx_D),
    .QAIDX_D(RS_D), 
    .QBIDX_D(RT_D),  
    .ALUC_E(AluC_E), 
    .ALUB_E(AluB_E), 
    .M2R_E(M2R_E), 
    .RWRT_E(RWrt_E), 
    .MWRT_E(MWrt_E), 
    .BREN_E(BrEn_E), 
    .QA_E(QA_E), 
    .QB_E(QB_E), 
    .PCP4_E(PCP4_E), 
    .IMM_E(Imm_E), 
    .WIDX_E(WIdx_E), 
    .QAIDX_E(QAIdx_E), 
    .QBIDX_E(QBIdx_E)
);

wire[31:0] AluOut_E;
wire EQ_E;

ALU alu(
    .A(BypassQA ? QA_Bypass : QA_E), 
    .B(AluB_E ? Imm_E : (BypassQB ? QB_Bypass : QB_E)), 
    .FUNC(AluC_E), 
    .OUT(AluOut_E), 
	//.LT(LT), 
	//.GT(GT), 
	.EQ(EQ_E)
);

wire BranchTaken_E;
assign BranchTaken_E = BrEn_E & EQ_E;

wire[31:0] PCBR_E;
assign PCBR_E = PCP4_E + (Imm_E << 2);

// Stage IV: Memory

wire[31:0] AluOut_M, QB_M, PCBR_M;
wire[4:0] WIdx_M, QAIdx_M, QBIdx_M;
wire RWrt_M, MWrt_M, M2R_M, BranchTaken_M;

SegRegsEXE2MEM EXE2MEM(
    .CLK(EffClk), 
    .RST(RST | SegRegsRstE2M), 
    .WRT(SegRegsWrtE2M), 
    .RWRT_E(RWrt_E), 
    .MWRT_E(MWrt_E), 
    .M2R_E(M2R_E), 
    .BRTKN_E(BranchTaken_E), 
    .WIDX_E(WIdx_E), 
    .QAIDX_E(QAIdx_E), 
    .QBIDX_E(QBIdx_E), 
    .QB_E(QB_E),
    .PCBR_E(PCBR_E), 
    .ALUOUT_E(AluOut_E), 
    .RWRT_M(RWrt_M), 
    .MWRT_M(MWrt_M), 
    .M2R_M(M2R_M), 
    .BRTKN_M(BranchTaken_M), 
    .WIDX_M(WIdx_M), 
    .QAIDX_M(QAIdx_M), 
    .QBIDX_M(QBIdx_M), 
    .QB_M(QB_M),
    .PCBR_M(PCBR_M), 
    .ALUOUT_M(AluOut_M)
);

assign BypassQA = RWrt_M & (QAIdx_M != 5'b0) & (QAIdx_M == WIdx_M);
assign BypassQB = RWrt_M & (QBIdx_M != 5'b0) & (QBIdx_M == WIdx_M);
assign BypassMem = (WIdx_M != 5'b0) & 1'b0;   // TODO

wire[31:0] BusDat_M, WDat_M;

assign DATADDR = AluOut_M;
assign DATOUT = MWrt_M ? QB_M : 32'hZZZZZZZZ;
assign BusDat_M = DATIN;
assign WDat_M = M2R_M ? BusDat_M : AluOut_M;
assign MWRT = MWrt_M;

assign PCBR_F = PCBR_M;
assign BranchTaken_F = BranchTaken_M;
assign Mem_Bypass = BusDat_M;

// Stage V: Write Back

wire[31:0] WDat_W;

SegRegsMEM2WB MEM2WB(
    .CLK(EffClk), 
    .RST(RST | SegRegsRstM2W), 
    .WRT(SegRegsWrtM2W), 
    .RWRT_M(RWrt_M), 
    .WIDX_M(WIdx_M), 
    .WDAT_M(WDat_M), 
    .RWRT_W(RWrt_W), 
    .WIDX_W(WIdx_W), 
    .WDAT_W(WDat_W)
);

assign WDat_D = BypassMem ? Mem_Bypass : WDat_W;

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
