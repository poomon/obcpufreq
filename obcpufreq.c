#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cpufreq.h>

#ifndef CPUFREQ_SET
#define CPUFREQ_SET "sudo -n cpufreq-set"
#endif

#define MAXLEN 20
#define LINE_LEN 10
#define	PIPEMENU_BEGIN printf("<?xml version='1.0' encoding='UTF-8'?>\n<openbox_pipe_menu>\n");
#define	PIPEMENU_END   printf("</openbox_pipe_menu>\n");
#define SEPARATOR      printf(" <separator/>\n");


static unsigned int count_cpus(void)
{
    FILE *fp;
    char value[LINE_LEN];
    unsigned int ret = 0;
    unsigned int cpunr = 0;

    fp = fopen("/proc/stat", "r");
    if(!fp) {
        // assume 1 cpu
        return 1;
    }

    while (!feof(fp)) {
        if (!fgets(value, LINE_LEN, fp))
            continue;
        value[LINE_LEN - 1] = '\0';
        if (strlen(value) < (LINE_LEN - 2))
            continue;
        if (strstr(value, "cpu "))
            continue;
        if (sscanf(value, "cpu%d ", &cpunr) != 1)
            continue;
        if (cpunr > ret)
            ret = cpunr;
    }
    fclose(fp);

    /* cpu count starts from 0, on error return 1 (UP) */
    return (ret+1);
}

void get_human_speed(char* dest, unsigned long speed)
{
    unsigned long tmp;

    if (speed > 1000000) {
        tmp = speed % 10000;
        if (tmp >= 5000)
            speed += 10000;
        snprintf (dest, MAXLEN, "%u.%02u GHz", ((unsigned int) speed/1000000),
            ((unsigned int) (speed%1000000)/10000));
    } else if (speed > 100000) {
        tmp = speed % 1000;
        if (tmp >= 500)
            speed += 1000;
        snprintf (dest, MAXLEN, "%u MHz", ((unsigned int) speed / 1000));
    } else if (speed > 1000) {
        tmp = speed % 100;
        if (tmp >= 50)
            speed += 100;
        snprintf (dest, MAXLEN, "%u.%01u MHz", ((unsigned int) speed/1000),
            ((unsigned int) (speed%1000)/100));
    } else
        snprintf (dest, MAXLEN, "%lu kHz", speed);

    return;
}

void print_cpu(unsigned int cpu, unsigned int n_cpus)
{
    int i = 0;
    char str[MAXLEN];
    char premark[MAXLEN];
    char postmark[MAXLEN];
    unsigned long curfreq, freq;
    struct cpufreq_available_frequencies* freqs;
    struct cpufreq_available_governors* govs;
    struct cpufreq_policy* policy;

    if (cpu + 1 == n_cpus)
        // last cpu column
        printf("<item label='cpu %u'/>\n", cpu);
    else if (cpu + 1 < n_cpus)
    {
        printf("<menu label='cpu %u' id='obcpufreq-cpu%u'>\n", cpu, cpu + 1);
        print_cpu(cpu + 1, n_cpus);
        printf("</menu>\n");
    }
    SEPARATOR

    curfreq = cpufreq_get_freq_kernel(cpu);
    policy = cpufreq_get_policy(cpu);

    // print available governors
    govs = cpufreq_get_available_governors(cpu);
    if (govs)
    {
        printf("<menu label='governors' id='obcpufreq-governors%u'>\n", cpu);
        for (; govs; govs = govs->next)
        {
            premark[0] = ' ';
            premark[1] = '\0';
            if (!strcmp(govs->governor, policy->governor))
                premark[0] = '*';
            printf("  <item label='%s%s'><action name='Execute'><command>"
                   CPUFREQ_SET " --cpu %u -g %s</command></action></item>\n",
                   premark, govs->governor,
                   cpu, govs->governor);
        }
        printf("</menu>\n");
        cpufreq_put_available_governors(govs);
    }
    else
        printf("<item label='%s'/>\n", "no available governors");
    SEPARATOR

    // print available freqs
    freqs = cpufreq_get_available_frequencies(cpu);
    if (freqs)
    {
        for (; freqs; freqs = freqs->next, i++)
        {
            freq = freqs->frequency;
            get_human_speed(str, freq);
            premark[0] = ' ';
            premark[1] = '\0';
            if (freq == curfreq)
                premark[0] = '*';

            postmark[0] = '\0';
            if (policy)
            {
                if (freq == policy->max && freq == policy->min)
                    strcpy(postmark, " (max,min)");
                else if (freq == policy->max)
                    strcpy(postmark,  " (max)");
                else if (freq == policy->min)
                    strcpy(postmark, " (min)");
            }

            printf("<menu label='%s%s%s' id='obcpufreq-freq%d'>\n"
                   "  <item label='set as max'><action name='Execute'><command>"
                       CPUFREQ_SET " --cpu %u --max %lu</command></action></item>\n"
                   "  <item label='set as min'><action name='Execute'><command>"
                       CPUFREQ_SET " --cpu %u --min %lu</command></action></item>\n</menu>\n",
                   premark, str, postmark, i,
                   cpu, freqs->frequency,
                   cpu, freqs->frequency);
        }
        cpufreq_put_available_frequencies(freqs);
    }
    else
        printf("<item label='%s'/>\n", "no available freqs");

    cpufreq_put_policy(policy);
    return;
}

int main(int argc, char** argv)
{
    PIPEMENU_BEGIN
    print_cpu(0, count_cpus());
    PIPEMENU_END
    return 0;
}

