CC = gcc
CFLAGS = -O2 -march=native
CPUFREQ_SET = sudo -n cpufreq-set

obcpufreq: obcpufreq.c
	$(CC) $(CFLAGS) -D'CPUFREQ_SET="$(CPUFREQ_SET)"' -lcpufreq -o $@ $^

clean:
	@rm -f obcpufreq
