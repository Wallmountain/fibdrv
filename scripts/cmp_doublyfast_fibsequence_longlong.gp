reset
set xlabel 'F(n)'
set ylabel 'time (ns)'
set title 'Fibonacci runtime'
set term png enhanced font 'Verdana,10'
set output 'cmp_fast_doubling_fibsequence_longlong.png'                                         
set grid
plot [0:92][0:300] \
'kernel_fib.txt' using 1:2 with linespoints linewidth 2 title "fib sequence with long long time",\
'kernel_fib_fast_doubling_longlong.txt' using 1:2 with linespoints linewidth 2 title "fast doubling with long long time"

