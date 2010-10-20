#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cpufreq.h>

/* user config 
 * ===========
 * %lu will be replaced by target frequency
 * %s will be replaced by target governor
 */
#define CPU 0
#define CPUFREQ_SET_MAX "sudo -n cpufreq-set --max %lu"
#define CPUFREQ_SET_MIN "sudo -n cpufreq-set --min %lu"
#define CPUFREQ_SET_GOV "sudo -n cpufreq-set -g %s"

#define MAXLEN 20
#define	PIPEMENU_BEGIN printf("<?xml version='1.0' encoding='UTF-8'?>\n<openbox_pipe_menu>\n");
#define	PIPEMENU_END   printf("</openbox_pipe_menu>\n");
#define SEPARATOR      printf(" <separator/>\n");

// ripped from cpufreq-info
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

int main(int argc, char** argv)
{
    int i = 0;
    char str[MAXLEN];
    char premark[MAXLEN];
    char postmark[MAXLEN];
    unsigned long curfreq, freq;
    struct cpufreq_available_frequencies* freqs;
    struct cpufreq_available_governors* govs;
    struct cpufreq_policy* policy;

    freqs = cpufreq_get_available_frequencies(CPU);
    govs = cpufreq_get_available_governors(CPU);
    policy = cpufreq_get_policy(CPU);
    curfreq = cpufreq_get_freq_kernel(CPU);

    PIPEMENU_BEGIN
    // print available governors
    if (govs)
    {
        printf("<menu label='governors' id='obcpufreq-governors'>\n");
        for (; govs; govs = govs->next)
        {
            premark[0] = ' ';
            premark[1] = '\0';
            if (!strcmp(govs->governor, policy->governor))
                premark[0] = '*';
            printf("  <item label='%s%s'><action name='Execute'><command>" CPUFREQ_SET_GOV
                   "</command></action></item>\n",
                   premark, govs->governor, govs->governor);
        }
        printf("</menu>\n");
        SEPARATOR
    }

    /*
    // print current freq
    get_human_speed(str, curfreq);
    printf("<item label='%s'/>\n", str);
    */

    // print available freqs
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
                   "  <item label='set as max'><action name='Execute'><command>" CPUFREQ_SET_MAX "</command></action></item>\n"
                   "  <item label='set as min'><action name='Execute'><command>" CPUFREQ_SET_MIN "</command></action></item>\n"
                   "</menu>\n",
                   premark, str, postmark, i,
                   freqs->frequency, freqs->frequency);
        }
    }
    else
        printf("<item label='%s'/>\n", "no available freqs");

    PIPEMENU_END
    return 0;
}

