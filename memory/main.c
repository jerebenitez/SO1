#include <stdio.h>
#include <stdlib.h>
#include "bloque.c"

void main()
{
  void *p0;
  size_t s0 = 5;
  size_t s00 = 7;

  void *p1;
  size_t s1 = 6;
  size_t s10 = 1;

  void *p2;
  size_t s2 = 2;
  size_t s20 = 10;

  void *p3;
  size_t s3 = 5;
  size_t s30 = 5;


  printf("\nTamaño del bloque: %lx\n", BLOCK_SIZE);

  printf("\nmalloc: Ubicacion del bloque:   %p\n", p0 = malloc(s0));
  printf("\nmalloc: Ubicacion del bloque:   %p\n", p1 = malloc(s1));
  printf("\nmalloc: Ubicacion del bloque:   %p\n", p2 = malloc(s2));
  printf("\nmalloc: Ubicacion del bloque:   %p\n", p3 = malloc(s3));

/*
  printf("\nmalloc: Ubicacion del bloque:   %d\n", p0 = (int) malloc(s0));
  printf("\nmalloc: Ubicacion del bloque:   %d\n", p1 = (int) malloc(s1));
  printf("\nmalloc: Ubicacion del bloque:   %d\n", p2 = (int) malloc(s2));
  printf("\nmalloc: Ubicacion del bloque:   %d\n", p3 = (int) malloc(s3));
*/
  void *ptr0 = realloc(p0, s00);
  void *ptr1 = realloc(p1, s10);
  void *ptr2 = realloc(p2, s20);
  void *ptr3 = realloc(p3, s30);

  free(p0);
  free(p1);
  free(p2);
  free(p3);
}
