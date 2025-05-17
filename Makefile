all: top.v sim_main.cpp
	verilator --cc --exe --build --trace --top-module top -j 6 -Wall sim_main.cpp top.v
	./obj_dir/Vtop
	gtkwave sim.vcd

