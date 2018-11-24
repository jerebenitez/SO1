#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_LENGTH 80               ///< Longitud del buffer
static char recieve[BUFFER_LENGTH];     ///< Buffer modificado por el modulo a llamar

int main()
{
   int openE, openD;
   char stringToSend[BUFFER_LENGTH];

   openE = open("/dev/encriptador", O_RDWR);             // Abro el archivo con permisos de lectoescritura
   if (openE < 0)
   {
      printf("\nError al abrir encriptador\n");
      return errno;
   }

   openD = open("/dev/desencriptador", O_RDWR);             // Abro el archivo con permisos de lectoescritura
   if (openD < 0)
   {
      perror("\nError al abrir desencriptador\n");
      return errno;
   }

   printf("Ingrese la frase a encriptar ('exit' para salir):\n\n-> ");

   while(1)
    {
      int ret, flag=1, result;

      result = scanf("%[^\n]%*c", stringToSend);                // Leo un string con espacios

      flag = strcmp(stringToSend,"exit");
      if(flag == 0)
         break;

      ret = write(openE, stringToSend, strlen(stringToSend)); // Envio el string al encriptador escribiendo en el archivo dev

      if (ret < 0)
      {
        printf("Error al escribir encriptador\n");
        return errno;
      }
      ret = read(openE, recieve, BUFFER_LENGTH);        // Leo la respuesta del modulo

      if (ret < 0)
        return errno;

      printf("\nEl mensaje encriptado es: %s\n", recieve);

      strcpy(stringToSend, recieve);

      ret = write(openD, stringToSend, strlen(stringToSend)); // Envio el string al encriptador escribiendo en el archivo dev
      if (ret < 0)
      {
         perror("Error al escribir desencriptador\n");
         return errno;
      }

      printf("\nPresione ENTER para ver el resultado\n");
      getchar();

      ret = read(openD, recieve, BUFFER_LENGTH);        // Leo la respuesta del modulo
      if (ret < 0)
      {
         perror("Error al leer desencriptador\n");
         return errno;
      }
      printf("El mensaje desencriptado es: %s\n", recieve);
      printf("\n=========================================================\n");
      printf("\nPor favor ingrese otro mensaje ('exit' para salir):\n\n->");
   }

   close(openE);
   close(openD);
   return 0;
}
