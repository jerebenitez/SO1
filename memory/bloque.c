#include <stddef.h>
#include <sys/types.h>
#include <unistd.h>

#define align4(x) (((((x)-1)>>2)<<2)+4)
#define BLOCK_SIZE sizeof(struct s_block)
//#define BLOCK_SIZE 20

typedef struct s_block *t_block;

struct s_block
{
  size_t size;
  struct s_block *next;
  struct s_block *prev;
  int free;
  void *ptr;    // ptr al bloque alocado
  char data[1];
};

void *base = NULL;

t_block find_block(t_block *last, size_t size)
{                                                 // encuentra t_block
  t_block b = base;                               // donde guardar
  while(b && !(b->free && b->size >= size))       // nuevo bloque
  {
    *last = b;
    b = b->next;
  }
  return (b);
}

t_block extend_heap(t_block last, size_t s)
{                                             // mueve el ptr brk
  int sb;                                     // de acuerdo al tamaño
  t_block b;                                  // requerido
  b = sbrk(0);

  sb = (int)sbrk(BLOCK_SIZE + s);
  if(sb < 0)
    return (NULL);

  b->size = s;
  b->next = NULL;
  b->prev = last;
  b->ptr = b->data;

  if(last)
    last->next = b;

  b->free = 0;
  return (b);
}

t_block split_block(t_block b, size_t s)
{                                               // divide bloque b
  t_block new;                                  // en dos bloques

  new = (t_block)(b->data + s);
  new->size = b->size - s - BLOCK_SIZE;
  new->next = b->next;
  new->prev = b;
  new->free = 1;
  new->ptr = new->data;

  b->size = s;
  b->next = new;

  if(new->next)
    new->next->prev = new;
}

void *malloc(size_t size)
{
  t_block b, last;
  size_t s;
  s = align4(size);

  if(base)
  {                    // base ocupado
      last = base;
      b = find_block(&last, s);
      if(b)
      {
        if((b->size - s) >= (BLOCK_SIZE + 4))
          split_block(b, s);
        b->free = 0;
      }
      else
      {
        b = extend_heap(last, s);
        if(!b)
          return (NULL);
      }
  }
  else
  {                        // base vacio => primera vez
    b = extend_heap(NULL, s);
    if(!b)
      return (NULL);
    base = b;
  }
  return (b->data);
}

t_block fusion(t_block b)
{                                         // fusiona dos bloques libres
  if(b->next && b->next->free)            // para evitar fragmentacion
  {
    b->size += BLOCK_SIZE + b->next->size;
    b->next = b->next->next;
    if(b->next)
      b->next->prev = b;
  }
  return (b);
}

t_block get_block(void *p)
{                                         // devuelve puntero apuntando
  char *tmp;                              // al inicio del bloque
  tmp = p;
  return (p = tmp -= BLOCK_SIZE);
}

int valid_addr(void *p)
{                                         // chequea si el addr
  if(base)                                // es valido para el free
  {
    if(p > base && p < sbrk(0))
      return (p == (get_block(p))->ptr);
  }
  return (0);
}

void free(void *p)
{
  t_block b;
  if (valid_addr(p))
  {
    b = get_block(p);
    b->free = 1;

    if(b->prev && b->prev->free)  // si posible, fusion con prev
      b = fusion(b->prev);

    if(b->next)  // si posible, fusion con next
      fusion(b);
    else
    {
      if (b->prev)
        b->prev->next = NULL;
      else
        base = NULL;
      brk(b);
    }
  }
}

void copy_block(t_block src, t_block dst)
{
  int *sdata, *ddata;
  size_t i;
  sdata = src->ptr;
  ddata = dst->ptr;

  for(i = 0; i*4 < src->size && i*4 < dst->size; i++)
    ddata[i] = sdata[i];
}

void *realloc(void *p, size_t size)
{

  size_t s;
  t_block b, new;
  void *newp;

  if (!p)
    return (malloc(size));    // realloc(NULL, s) = malloc(s)

  if(valid_addr(p))
  {
      s = align4(size);
      b = get_block(p);
      if(b->size >= s)
      {
        if(b->size - s >= (BLOCK_SIZE + 4))
          split_block(b, s);
      }
      else
      {
        if(b->next && b->next->free &&         // try fusion con el siguiente
           (b->size + BLOCK_SIZE + b->next->size) >= s)
           {
             fusion(b);
             if(b->size - s >= (BLOCK_SIZE + 4))
              split_block(b, s);
           }
        else
        {
          newp = malloc(s);
          if(!newp)
            return (NULL);

          new = get_block(newp);
          copy_block(b, new);
          free(p);
          return (newp);
        }
      }
      return (p);
  }
  return (NULL);
}
