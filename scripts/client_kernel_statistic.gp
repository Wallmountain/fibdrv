reset
set xlabel 'F(n)'
set ylabel 'time (ns)'
set title 'Fibonacci runtime'
set term png enhanced font 'Verdana,10'
set output 'kernel_client_fib_sequence.png'
set grid
plot [0:92][0:2000] \
'kernel_fib.txt' using 1:2 with linespoints linewidth 2 title "fib sequence time",\
'client_time.txt' using 1:2 with linespoints linewidth 2 title "client time"
