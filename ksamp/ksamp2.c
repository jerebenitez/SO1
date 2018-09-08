#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>

struct Uptime {
  int days;
  int hours;
  int minutes;
  float seconds;
};

char* get_kernel_version () {
    FILE* fp;
    char buffer[1024];
    size_t bytes_read;
    char* match;
    char* kernel_version;

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

char* get_filesystems () {
  FILE* fp;
  char buffer[1024];
  size_t bytes_read;
  char* sysfile;

  fp = fopen("/proc/filesystems", "r");
  bytes_read = fread(buffer, 1, sizeof(buffer), fp);
  fclose(fp);

  if(bytes_read == 0 || bytes_read == sizeof(buffer))
    return 0;

  buffer[bytes_read] = '\0';

  sscanf(buffer, "nodev %s", sysfile);
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

int main(int argc, char *argv[]){
    /////////////////////////////////////////////////
    printf("Hostname: %s", get_hostname());
    ///////////////////////////////////////////////////
    printf("Time: %s", get_time());
    /////////////////////////////////////////////////////
    // INSTANCIA Uptime
    struct Uptime ut = {
      .seconds = 0,
      .days = 0,
      .hours = 0,
      .minutes = 0,
    };
    // LECTURA DE SEGUNDOS (LEER get_seconds_up())
    float tmp_seconds;
    tmp_seconds = atof(get_seconds_up()); // atof() - de char a float
    // PASAJE DE SEGUNDOS A FORMATO FECHA
    ut.days = tmp_seconds / 86400;
    ut.hours = tmp_seconds / 3600 - ut.days * 24;
    ut.minutes = tmp_seconds / 60 - ut.hours * 60;
    ut.seconds = tmp_seconds - ut.days * 86400 - ut.hours * 3600 - ut.minutes * 60;
    // IMPRIME TIEMPO ENCENDIDO
    printf("Uptime: %iD %i/%i/%.2f  Upseconds: %.2f \n", ut.days, ut.hours, ut.minutes, ut.seconds , tmp_seconds);

    /////////////////////////////////////////////////////
    printf("Kernel version: %s \n", get_kernel_version());
    /////////////////////////////////////////////////////
    //    printf("Sysfile %s\n", get_filesystems());

    printf("CPU type%s \n", get_cpu_type());
    printf("CPU model%s \n", get_cpu_model());
    return 0;
}
