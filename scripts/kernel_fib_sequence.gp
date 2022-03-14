reset
set xlabel 'F(n)'
set ylabel 'time (ns)'
set title 'Fibonacci runtime'
set term png enhanced font 'Verdana,10'
set output 'kernel_fib_sequence.png'
set grid
plot [0:92][0:300] \
'kernel_fib.txt' using 1:2 with linespoints linewidth 2 title "fib sequence time"

