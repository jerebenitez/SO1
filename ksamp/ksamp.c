#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include<getopt.h>

#include "ksamp.h"

int main(int argc, char *argv[]){
    float tmp_seconds;
    int next_option;
    // Options flags
    int stats = 0, interval_duration = 0;
    
    const char* const short_opts = "sI:";
    const struct option long_opts [] = {
      {"stat",              no_argument,       NULL, 's'},
      {"interval-duration", required_argument, NULL, 'I'},
      {0,                   0,                 0,    0},
    };
    
    while ((next_option = getopt_long(argc, argv, short_opts, long_opts, NULL)) != -1) {
        switch (next_option) {
            case 'I':                
                interval_duration = 1;
                break;
            case 's':
                stats = 1;
                break;
            case 'h':
            case '?':
            default:
                print_usage();
                return 0;
        }
    }
    
    if (interval_duration + stats > 1) {
        print_usage();
        return 0;
    } else if (interval_duration == 1) {
        get_s_options();
        get_interval_duration(1, 1);
    } else if (stats == 1) {
        get_s_options();
    }

    struct Uptime ut = { //INSTANCIA UPTIME
        .seconds = 0,
        .days = 0,
        .hours = 0,
        .minutes = 0,
    };
    // LECTURA DE SEGUNDOS (LEER get_seconds_up())
    tmp_seconds = atof(get_seconds_up()); // atof() - de char a float
    // PASAJE DE SEGUNDOS A FORMATO FECHA
    ut.days = tmp_seconds / 86400;
    ut.hours = tmp_seconds / 3600 - ut.days * 24;
    ut.minutes = tmp_seconds / 60 - ut.hours * 60;
    ut.seconds = tmp_seconds - ut.days * 86400 - ut.hours * 3600
                - ut.minutes * 60;

    printf("Hostname: %s", get_hostname());
    printf("Kernel version: %s \n", get_kernel_version());
    printf("Time: %s", get_time());
    printf("Uptime: %iD %i/%i/%.2f  Upseconds: %.2f \n", ut.days,
    ut.hours, ut.minutes, ut.seconds , tmp_seconds);
    printf("CPU type%s \n", get_cpu_type());
    printf("CPU model%s \n", get_cpu_model());
    printf("Allowed filesystems quantity: %i\n", get_filesystems());

    return 0;
}

void print_usage() {
    printf("--stat \t-s :\tprint system stats\n");
    printf("--interval-duration \t-I <seg> <min>:\tprint interval duration\n");
}

void get_file_as_string(char* file_name, char** buffer) {
    FILE* fp = fopen(file_name, "r");
    if (fp == NULL)
        return;
    
    size_t len;
    ssize_t bytes_read = getdelim(buffer, &len, '\0', fp);
    
    fclose(fp);
    
    if (bytes_read == -1)
        buffer = NULL;
}

char* get_kernel_version () {
    char* buffer = NULL;
    char* match;
    char* kernel_version;
    
    //memset(&kernel_version[0], 0, sizeof(kernel_version));
    
    get_file_as_string("/proc/version", &buffer);
    if (buffer == NULL)
        return "Can't read version.";

    match = strstr(buffer, "Linux version");

    if (match == NULL)
        return "Can't find version.";

    sscanf(match, "Linux version %s", kernel_version);
    return kernel_version;
}

char* get_seconds_up() {
    char* buffer = NULL;
    char* seconds_up;
    
    get_file_as_string("/proc/uptime", &buffer);
    if (buffer == NULL)
        return "Can't read uptime.";

    seconds_up = strtok(buffer, " ");   
    return seconds_up;
}

char* get_time() {
    time_t curtime;
    time(&curtime);
    return ctime(&curtime);
}

char* get_hostname () {
    char* buffer = NULL;
    char* hostname;
    
    get_file_as_string("/proc/sys/kernel/hostname", &buffer);
    if (buffer == NULL)
        return "Can't read hostname.";

    hostname = strtok(buffer, "");
    return hostname;
}

int get_filesystems () {
    char* buffer = NULL;
    int c = 0;
    int sysfile = 0;
    
    get_file_as_string("/proc/filesystems", &buffer);
    if (buffer == NULL)
        return -1;
    
    while (buffer[c] != '\0') {
        if (buffer[c] == '\n') {
            sysfile++;
            c++;
        } else {
            c++;
        }
    }

    return sysfile;
}

char* get_cpu_model() {
    char* buffer = NULL;
    char* match;
    char* matchaux;
    char* model;
    
    get_file_as_string("/proc/cpuinfo", &buffer);
    if (buffer == NULL)
        return "Can't read cpuinfo.";

    match = strstr(buffer, "model name");
    if (match == NULL)
        return "Can't find value.";

    matchaux = strstr(match, ":");
    model = strtok(matchaux, "\n");

    return model;
}

char* get_cpu_type() {
    char* buffer = NULL;
    char* match;
    char* matchaux;
    char* type;
    
    get_file_as_string("/proc/cpuinfo", &buffer);
    if (buffer == NULL)
        return "Can't read cpuinfo.";

    match = strstr(buffer, "vendor_id");
    if (match == NULL)
        return "Can't find value.";

    matchaux = strstr(match, ":");
    type = strtok(matchaux, "\n");

    return type;
}

void get_s_options() {
    char user[10], nice[10], sys[10], idle[10], ctxt[10],
         processes[10];
    char* buffer = NULL;
    
    get_file_as_string("/proc/stat", &buffer);
    if (buffer == NULL) {
        printf("Could not load stats.");
        return;
    }

    printf("STAT OPTIONS \n");
    // ESCANEO E IMPRESION CPU TIME
    sscanf(buffer, "cpu %s %s %s %s", user, nice, sys, idle);
    printf("CPU Time: User: %s System: %s Idle: %s\n", user, sys, idle);
    // ESCANEO E IMPRESION CTXT SWITCHES
    char* ctxt_tmp = strstr(buffer, "ctxt");
    sscanf(ctxt_tmp, "ctxt %s", ctxt);
    printf("Context switches: %s\n", ctxt);

    char* processes_tmp = strstr(buffer, "processes");
    sscanf(processes_tmp, "processes %s", processes);
    printf("Processes: %s\n\n", processes);
}

void get_interval_duration(int a, int b) {
    return;
}
