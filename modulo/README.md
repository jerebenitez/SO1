# SO1 2018 - Trabajo práctico 4

## Integrantes
* Benítez, Darío Jeremías

* Sarquis, Tomás

## Introducción
Este programa consta de dos modulos kernel: un encriptador, al cual se le envía un mensaje y éste lo encripta, y un desencriptador,
el cual recibe el mensaje ya encripado y nos devuelve el original. 

## Instalación y uso del programa
1. Descargar y extraer repositorio
2. En consola, ir a modulo/encriptador y ejecutar:   

**$ make
**$ sudo insmod encriptador.ko

3. En consola, ir a modulo/desencriptador y ejecutar:   make
                                                        sudo insmod desencriptador.ko
4. En consola, ir a modulo/user y ejecutar:   make
                                              sudo ./user
5. Seguir instrucciones del programa
6. Al finalizar, ejecutar en consola:   sudo rmmod encriptador
                                        sudo rmmod desencriptador


## Conclusión
La realización del trabajo nos permitió comprender el procedimiento a la hora de asignar memoria, tanto desde el punto de vista lógico (algorítmico) como desde el punto de vista de su implementación en C. A su vez, apreciamos la importancia del manejo de posibles errores y el tratado de los mismos, debido a que un error en éste tipo de programas puede ser fatal para el mismo.
