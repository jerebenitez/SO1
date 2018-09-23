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
    const char* const short_options = "s";  // OPCIONES CORTAS
    const struct option long_options [] = {   // OPCIONES LARGAS,
      {"stat", 0, NULL, 's'},
      {NULL, 0, NULL, 0},
    };
    do {
      next_option = getopt_long (argc, argv, short_options,
                                 long_options, NULL);

        switch (next_option) {
            case 's':
                get_s_options();
                break;
            case '?':
                break;
            case -1:
                break;
            default:
                abort();
        }} while (next_option != -1);

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
    FILE* fp;
    char buffer[1024];
    size_t bytes_read;
    char* match;
    char kernel_version[100];
    
    memset(&kernel_version[0], 0, sizeof(kernel_version));

    fp = fopen("/proc/version", "r");
    bytes_read = fread(buffer, 1, sizeof(buffer), fp);
    fclose(fp);

    if(bytes_read == 0 || bytes_read == sizeof(buffer))
        return 0;

    buffer[bytes_read] = '\0';

    match = strstr(buffer, "Linux version");

    if (match == NULL)
        return 0;

    sscanf(match, "Linux version %s", kernel_version);
    return kernel_version;
}

char* get_seconds_up() {
    FILE* fp;
    char buffer[1024];
    size_t bytes_read;
    char* seconds_up;

    fp = fopen("/proc/uptime", "r");
    bytes_read = fread(buffer, 1, sizeof(buffer), fp);
    fclose(fp);

    if(bytes_read == 0 || bytes_read == sizeof(buffer))
        return 0;

    buffer[bytes_read] = '\0';
    seconds_up = strtok(buffer, " ");
    
    return seconds_up;
}

char* get_time() {
    time_t curtime;
    time(&curtime);
    return ctime(&curtime);
}

char* get_hostname () {
    FILE* fp;
    char buffer[1024];
    size_t bytes_read;
    char* hostname;

    fp = fopen("/proc/sys/kernel/hostname", "r");
    bytes_read = fread(buffer, 1, sizeof(buffer), fp);
    fclose(fp);

    if(bytes_read == 0 || bytes_read == sizeof(buffer))
        return 0;

    buffer[bytes_read] = '\0';

    hostname = strtok(buffer, "");
    return hostname;
}

int get_filesystems () {
    FILE* fp;
    char buffer[1024];
    size_t bytes_read;
    int c = 0;
    int sysfile = 0;

    fp = fopen("/proc/filesystems", "r");
    bytes_read = fread(buffer, 1, sizeof(buffer), fp);
    fclose(fp);

    if(bytes_read == 0 || bytes_read == sizeof(buffer))
        return 0;

    buffer[bytes_read] = '\0';

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
    FILE* fp;
    char buffer[10000];
    size_t bytes_read;
    char* match;
    char* matchaux;
    char* model;

    fp = fopen("/proc/cpuinfo", "r");
    bytes_read = fread(buffer, 1, sizeof(buffer), fp);

    if (bytes_read == 0 || bytes_read == sizeof(buffer))
        return 0;

    buffer[bytes_read] = '\0';

    match = strstr(buffer, "model name");
    if (match == NULL)
        return "Can't find value.";

    matchaux = strstr(match, ":");
    model = strtok(matchaux, "\n");

    return model;
}

char* get_cpu_type() {
    FILE* fp;
    char buffer[10000];
    size_t bytes_read;
    char* match;
    char* matchaux;
    char* type;

    fp = fopen("/proc/cpuinfo", "r");
    bytes_read = fread(buffer, 1, sizeof(buffer), fp);

    if (bytes_read == 0 || bytes_read == sizeof(buffer))
        return 0;

    buffer[bytes_read] = '\0';

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
