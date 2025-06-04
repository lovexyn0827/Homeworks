onerror {resume}
quietly WaveActivateNextPane {} 0
add wave -noupdate /top_tb/CLK
add wave -noupdate /top_tb/RST
add wave -noupdate -childformat {{{/top_tb/uut/cpu/rf/regs[31]} -radix hexadecimal} {{/top_tb/uut/cpu/rf/regs[30]} -radix hexadecimal} {{/top_tb/uut/cpu/rf/regs[29]} -radix hexadecimal} {{/top_tb/uut/cpu/rf/regs[28]} -radix hexadecimal} {{/top_tb/uut/cpu/rf/regs[27]} -radix hexadecimal} {{/top_tb/uut/cpu/rf/regs[26]} -radix hexadecimal} {{/top_tb/uut/cpu/rf/regs[25]} -radix hexadecimal} {{/top_tb/uut/cpu/rf/regs[24]} -radix hexadecimal} {{/top_tb/uut/cpu/rf/regs[23]} -radix hexadecimal} {{/top_tb/uut/cpu/rf/regs[22]} -radix hexadecimal} {{/top_tb/uut/cpu/rf/regs[21]} -radix hexadecimal} {{/top_tb/uut/cpu/rf/regs[20]} -radix hexadecimal} {{/top_tb/uut/cpu/rf/regs[19]} -radix hexadecimal} {{/top_tb/uut/cpu/rf/regs[18]} -radix hexadecimal} {{/top_tb/uut/cpu/rf/regs[17]} -radix hexadecimal} {{/top_tb/uut/cpu/rf/regs[16]} -radix hexadecimal} {{/top_tb/uut/cpu/rf/regs[15]} -radix hexadecimal} {{/top_tb/uut/cpu/rf/regs[14]} -radix hexadecimal} {{/top_tb/uut/cpu/rf/regs[13]} -radix hexadecimal} {{/top_tb/uut/cpu/rf/regs[12]} -radix hexadecimal} {{/top_tb/uut/cpu/rf/regs[11]} -radix hexadecimal} {{/top_tb/uut/cpu/rf/regs[10]} -radix hexadecimal} {{/top_tb/uut/cpu/rf/regs[9]} -radix hexadecimal} {{/top_tb/uut/cpu/rf/regs[8]} -radix hexadecimal} {{/top_tb/uut/cpu/rf/regs[7]} -radix hexadecimal} {{/top_tb/uut/cpu/rf/regs[6]} -radix hexadecimal} {{/top_tb/uut/cpu/rf/regs[5]} -radix hexadecimal} {{/top_tb/uut/cpu/rf/regs[4]} -radix hexadecimal} {{/top_tb/uut/cpu/rf/regs[3]} -radix hexadecimal} {{/top_tb/uut/cpu/rf/regs[2]} -radix hexadecimal} {{/top_tb/uut/cpu/rf/regs[1]} -radix hexadecimal}} -expand -subitemconfig {{/top_tb/uut/cpu/rf/regs[31]} {-radix hexadecimal} {/top_tb/uut/cpu/rf/regs[30]} {-radix hexadecimal} {/top_tb/uut/cpu/rf/regs[29]} {-radix hexadecimal} {/top_tb/uut/cpu/rf/regs[28]} {-radix hexadecimal} {/top_tb/uut/cpu/rf/regs[27]} {-radix hexadecimal} {/top_tb/uut/cpu/rf/regs[26]} {-radix hexadecimal} {/top_tb/uut/cpu/rf/regs[25]} {-radix hexadecimal} {/top_tb/uut/cpu/rf/regs[24]} {-radix hexadecimal} {/top_tb/uut/cpu/rf/regs[23]} {-radix hexadecimal} {/top_tb/uut/cpu/rf/regs[22]} {-radix hexadecimal} {/top_tb/uut/cpu/rf/regs[21]} {-radix hexadecimal} {/top_tb/uut/cpu/rf/regs[20]} {-radix hexadecimal} {/top_tb/uut/cpu/rf/regs[19]} {-radix hexadecimal} {/top_tb/uut/cpu/rf/regs[18]} {-radix hexadecimal} {/top_tb/uut/cpu/rf/regs[17]} {-radix hexadecimal} {/top_tb/uut/cpu/rf/regs[16]} {-radix hexadecimal} {/top_tb/uut/cpu/rf/regs[15]} {-radix hexadecimal} {/top_tb/uut/cpu/rf/regs[14]} {-radix hexadecimal} {/top_tb/uut/cpu/rf/regs[13]} {-radix hexadecimal} {/top_tb/uut/cpu/rf/regs[12]} {-radix hexadecimal} {/top_tb/uut/cpu/rf/regs[11]} {-radix hexadecimal} {/top_tb/uut/cpu/rf/regs[10]} {-radix hexadecimal} {/top_tb/uut/cpu/rf/regs[9]} {-radix hexadecimal} {/top_tb/uut/cpu/rf/regs[8]} {-radix hexadecimal} {/top_tb/uut/cpu/rf/regs[7]} {-radix hexadecimal} {/top_tb/uut/cpu/rf/regs[6]} {-radix hexadecimal} {/top_tb/uut/cpu/rf/regs[5]} {-radix hexadecimal} {/top_tb/uut/cpu/rf/regs[4]} {-radix hexadecimal} {/top_tb/uut/cpu/rf/regs[3]} {-radix hexadecimal} {/top_tb/uut/cpu/rf/regs[2]} {-radix hexadecimal} {/top_tb/uut/cpu/rf/regs[1]} {-radix hexadecimal}} /top_tb/uut/cpu/rf/regs
add wave -noupdate -radix hexadecimal /top_tb/uut/cpu/PC
add wave -noupdate -radix hexadecimal /top_tb/uut/cpu/Insn_D
TreeUpdate [SetDefaultTree]
WaveRestoreCursors {{Cursor 1} {0 ps} 0}
quietly wave cursor active 0
configure wave -namecolwidth 150
configure wave -valuecolwidth 100
configure wave -justifyvalue left
configure wave -signalnamewidth 0
configure wave -snapdistance 10
configure wave -datasetprefix 0
configure wave -rowmargin 4
configure wave -childrowmargin 2
configure wave -gridoffset 0
configure wave -gridperiod 1
configure wave -griddelta 40
configure wave -timeline 0
configure wave -timelineunits ps
update
WaveRestoreZoom {0 ps} {1 ns}
