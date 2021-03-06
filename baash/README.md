# SO1 2018 - Trabajo práctico 2

## Integrantes
* Benítez, Darío Jeremías

* Sarquís, Tomás

## Introducción
Este trabajo tuvo múltiples objetivos: se buscó, por un lado, entender la programación multiproceso en C y, por el otro, aprender a usar redirección de file descriptors y pipes. Para lograr esto se implementó una versión limitada de un shell, la cual debía cumplir ciertos requisitos. Los mismos fueron entregados como parte de la consigna.

## Sobre el programa
### 1. Parte A

  La primer parte del trabajo pedía implementar simplemente el prompt, la capacidad de invocar comandos (sin utilizar execvp()) y la implementaciòn de dos comandos internos: *exit* y *cd*.
  El prompt fue programado para ser siempre el mismo, sin posibilidad de modificarlo mediante variables de entorno. EL mismo quedó implementado de la siguiente forma:
  ```C
  char cwd[PATH_MAX];
  char host[1024];
  getcwd(cwd, sizeof(cwd));
  gethostname(host, 1024);
  char *user = getenv("USER");
  
  printf("%s@%s %s$ ", user, host, cwd);
  ```
  dando como resultado, por ejemplo: **jere@archlinux /home/jere$**
  
  La implementación de la ejecución de los comandos puede verse más claramente en la función *invoke()*. En términos generales lo que se hace es:
  + Si *programa* (la variable donde está almacenado el string con el comando a ejecutar) empieza con / o con ./ se pasa directamente a *execv*, junto con los parámetros introducidos por el usuario. En caso de no encontrar dicho programa, se muestra un error en la pantalla.
  + Si *programa* no comienza con / o con ./ entonces hay dos posibilidades: o el programa está en alguno de los directorios apuntados en $PATH, o se está haciendo un direccionamiento relativo con el ./ implícito. 
    + Para probar el primer caso se itera a través de todos los directorios en $PATH, generando un nuevo string concatenando *path* con *programa*. Luego dicho string es pasado a *execv*. Si ninguna de las iteraciones encuentra al programa buscado, entocnes es el segundo caso.
    + Para este caso se pasa directamente *programa* a *execv* y se opera de manera similar al caso de la redirecciòn absoluta o relativa.
  
  Los comandos internos fueron implementados como casos especiales que se prueban antes de realizar el fork (para que se ejecuten en el proceso padre y no en los hijos).

### 2. Parte B

  La parte B pedía implementar la ejecución de procesos en segundo plano utilizando el delimitador &. Implementarlo fue simplemente agregar un campo en el struct *command_node* llamado is_concurrent, que se pone en 1 si el usuario poveyó dicho delimitador. Luego del fork, en el proceso padre, se pregunta el estado de este flag (técnicamente se pregunta el estado de este flag para cualquier comando en una cadena de pipes ya que todos se toman como uno, más sobre eso en la parte C). Si el flag está en 1, entonces se espera a que los procesos hijos terminen con
  ```C
  waitpid(ret, &stat_loc, WUNTRACED);
  ```

### 3. Parte C

  La implementación de esta parte fue la que más tiempo y dificultad tuvo. En la especificación es pedido que el programa sea capaz de tratar solamente sentencias del tipo **cmd1 > file**, **cmd1 < file** ó **cmd1 | cmd2**; siendo parte del apartado extra el implementar la posibilidad de ejecutar algo que contenga los 4 delimitadores (&, |, < y >) en una misma línea. En esta implementación se intentó ir un poco más lejos y se logró el parseo y ejecución de líneas que contengan un número arbitrario de delimitadores (siempre que se respete una sintaxis primitiva), así como también se logró implementar un delimitador extra: **;**. El mismo sirve para separar, en una misma línea de comando, múltiples cadenas de ejecución. Esto permite que se ejecuten cosas de la forma **cmd1 | cmd2 | cmd3 > output ; cmd4 < input ; cmd5 | cmd6**.
  
  Esto fue realizado en dos etapas: primero una de parseo, y luego una de ejecuciòn.
  
  La etapa de parseo consiste en parsear la línea completa de comandos, generando una linked list de *command_node*s. Esto es un struct que se creó, que contiene la siguiente información, necesaria luego para la ejecución de los programas correctamente:
  ```C
  typedef struct command {
    char *command; // contiene el string con el comando a ejecutar, utilizado en invoke y en main para detectar comnados internos
    char **argv; // lista de parámetros para el comando pasados por el usuario, usado en execv()
    int argc; // cantidad de parámetros, no utilizado
    int is_concurrent; // flag que indica si el programa debe ser ejecutado en segundo plano o no
    int is_piped; // flag que indica si el programa lleva un pipe al programa siguiente en la lista
    char *input; // nombre del archivo del cual tomar el input en la redirección, indicado con <
    char *output; // nombre del archivo al cual mandar el output, indicado con >
    
    struct command *next; // puntero al siguiente comando a ejecutar
  } command_node;
  ```
  Una vez leída la línea, y generada la lista de nodos, se procede a la etapa de ejecución.
  
  El algoritmo de esta parte fue tomado de https://www.cs.purdue.edu/homes/grr/SystemsProgrammingBook/Book/Chapter5-WritingYourOwnShell.pdf (pag. 13). Algunas modificaciones debieron ser realizadas para que funcionara con la lista de *command_node*s.
  
  Una explicación más a fondo de cómo funciona puede ser obtenida de las páginas siguientes del pdf.

## Conclusión
Este trabajo nos permitió no sólo trabajar más a fondo tanto con los conceptos vistos en clase, como con C; sino que también nos llevó, debido a la investigación que conllevó implementar el parseador deseado, a comprender mejor el funcionamiento de linux. Los conceptos de pipes y redirecciones, que habían quedado no del todo entendidos, fueron finalmente comprendidos.
