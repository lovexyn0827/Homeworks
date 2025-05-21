#include "Vtop.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char** argv) {
    	VerilatedContext* contextp = new VerilatedContext;
		contextp->commandArgs(argc, argv);
    	Vtop* top = new Vtop{contextp};
		Verilated::traceEverOn(true);
		VerilatedVcdC* tfp = new VerilatedVcdC;
		top->trace(tfp, 99);
    	tfp->open("sim.vcd");
		int  i = 1000;
		top->RST = 1;
		top->CLK = 1;
		contextp->timeInc(5000);
		top->eval();
		tfp->dump(contextp->time());
		top->RST = 1;
		top->CLK = 0;
		contextp->timeInc(5000);
		top->eval();
		tfp->dump(contextp->time());
		top->RST = 1;
		top->CLK = 1;
		contextp->timeInc(5000);
		top->eval();
		tfp->dump(contextp->time());
		top->RST = 1;
		top->CLK = 0;
		contextp->timeInc(5000);
		top->eval();
		top->RST = 0;
		tfp->dump(contextp->time());
		while (i-- > 0) {
				top->CLK = !top->CLK;
				top->eval();
				tfp->dump(contextp->time());
				//assert(top->f == (a ^ b));
				contextp->timeInc(5000);
		} 

		tfp->close();
		delete top;
    delete contextp;
		return 0;
}

