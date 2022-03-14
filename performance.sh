#!/ bin / bash
#This script tweak the system setting to minimize the unstable factors while
#analyzing the performance of fibdrv.
#
# 2020 / 3 / 21 ver 1.0
CPUID=1
ORIG_ASLR=`cat /proc/sys/kernel/randomize_va_space`
ORIG_GOV=`cat /sys/devices/system/cpu/cpu$CPUID/cpufreq/scaling_governor`
ORIG_TURBO=`cat /sys/devices/system/cpu/intel_pstate/no_turbo`

sudo bash -c "echo 0 > /proc/sys/kernel/randomize_va_space"
sudo bash -c "echo performance > /sys/devices/system/cpu/cpu$CPUID/cpufreq/scaling_governor"
sudo bash -c "echo 1 > /sys/devices/system/cpu/intel_pstate/no_turbo"

#measure the performance of fibdrv
make all
make client_time
make kernel_fib
make unload
make load
sudo taskset -c $CPUID ./kernel_fib 0 > kernel_fib.txt
sudo taskset -c $CPUID ./client_time 0 > client_time.txt
sudo taskset -c $CPUID ./kernel_fib 1 > kernel_fib_fast_doubling_longlong.txt
gnuplot scripts/kernel_fib_sequence.gp
gnuplot scripts/client_kernel_statistic.gp
gnuplot scripts/cmp_doublyfast_fibsequence_longlong.gp
make unload
rm kernel_fib
rm kernel_fib.txt
rm client_time
rm client_time.txt
rm kernel_fib_fast_doubling_longlong.txt
make clean

#restore the original system settings
sudo bash -c "echo $ORIG_ASLR > /proc/sys/kernel/randomize_va_space"
sudo bash -c "echo $ORIG_GOV > /sys/devices/system/cpu/cpu$CPUID/cpufreq/scaling_governor"
sudo bash -c "echo $ORIG_TURBO > /sys/devices/system/cpu/intel_pstate/no_turbo"
