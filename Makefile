V_SOURCES=$(wildcard *.v)

all: $(V_SOURCES) sim_main.cpp
	verilator --cc --exe --build --trace --top-module top -j 6 -Wall sim_main.cpp $(V_SOURCES)
	./obj_dir/Vtop
	gtkwave sim.vcd

