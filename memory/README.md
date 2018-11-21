# SO1 2018 - Trabajo práctico 4

## Integrantes
* Benítez, Darío Jeremías

* Sarquis, Tomás

## Introducción
El objetivo de este práctico es comprender el uso y gestión de la memoria mediante las funciones de alocación, realocación y liberación de memoria (malloc, realloc y free, respectivamente). Para lograr dicho fin, se buscará realizar una implementacion propia de las funciones anteriormente mencionadas.

## Sobre el programa
Usando como guía el tutorial brindado por la cátedra, lo primero que se hizo fue implementar la estructura lógica de datos que se iba a utilizar, ésta es, la estructura de bloque. Dicha estructura contiene una primera parte, de tamaño fijo, que corresponde a metadatos, y una segunda destinada a los datos. 
Para poder asignar memoria de forma óptima, era necesario implementar además métodos de manejo de bloques. Gracias a dichos métodos, se pudo minimizar la fragmentación de la memoria y el mal uso de la misma, además de volver posible la asignación.
El algoritmo utilizado para la asignacion del espacio de memoria fue el first_fit, que se encarga de asignar el primer espacio de memoria compatible que encuentra.

## Conclusión
La realización del trabajo nos permitió comprender el procedimiento a la hora de asignar memoria, tanto desde el punto de vista lógico (algorítmico) como desde el punto de vista de su implementación en C. A su vez, apreciamos la importancia del manejo de posibles errores y el tratado de los mismos, debido a que un error en éste tipo de programas puede ser fatal para el mismo.
