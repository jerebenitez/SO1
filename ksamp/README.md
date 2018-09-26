# SO1 2018 - Trabajo práctico 1

## Integrantes
* Benítez, Darío Jeremías

* Sarquis, Tomás

## Introducción
El objetivo de este trabajo práctico es entender la estructura del kernel de Linux un poco más en profundidad, saliendo de lo puramente teórico. Para lograr este objetivo se realizó un programa que busca información específica del sistema utilizando los archivos encontrados en el directorio /proc.


## Sobre /proc

Este es un directorio virtual (los archivos acá encontrados son generados por el kernel en memoria), cuyo uso es presentar información del sistema (memoria disponible, procesos creados, etc.) así como también permitir la modificación de ciertos parámetros del kernel mediante la escritura de los archivos aquí encontrados *(para más información* man 5 proc *)*.

## Sobre el programa
El programa se realizó siguiendo las especificaciones dadas por la cátedra. Para la implementación se dividió el mismo en tres etapas, cada una correspondiente a las partes B, C, y D de la especificación. 

La primer etapa, en la que se implementó la parte B, se realizó utilizando pair programming para permitir asé que, luego, ambos alumnos supieran qué hacer, y tuvieran una idea similar de cómo hacerlo.

Las otras dos etapas fueron desarrolladas cada una por un alumno distinto.

Una vez que las tres etapas estuvieron funcionando, se procedió primero a refactorizar el código y, una vez que se comprobó que dicha refactorización no había roto el funcionamiento, realizó un checkeo utilizando cppcheck.

Para el control de versionado del proyecto se utilizó GitHub (el repo se encuentra [aquí](https://github.com/jerebenitez/SO1)).


## Conclusión
El proyecto sirvió tanto como para entender la estructura del kernel de linux (en términos un poco superficiales, pero suficientes) como para repasar programación en C. Los principales problemas que se presentaron fueron a la hora de trabajar con punteros (y punteros a punteros), pero todos fueron solucionados leyendo documentación y algunas respuestas en StackOverflow. No se logró implementar lo pedido en las Tareas Adicionales por falta de tiempo debida a la carga horaria de la facultad.
