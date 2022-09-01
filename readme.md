# Trabajo practico de la materia Sistemas Operativos, 1C 2022.

Consiste en un simulador de sistema operativo hecho en Linux. 
Simula el comportamiento de un SO en la ejecucion de instrucciones basicas, su planificacion y gestion de memoria. 
Se implementan comunicaciones mediante sockets entre los clientes/servidores: memoria, kernel, cpu y consolas. 
Se implementa ejecucion en multiples hilos en cada modulo. 

--------- Links ----------

Enunciado: https://docs.google.com/document/d/17WP76Vsi6ZrYlpYT8xOPXzLf42rQgtyKsOdVkyL5Jj0/edit

Pruebas: https://docs.google.com/document/d/1SBBTCweMCiBg6TPTt7zxdinRh4ealRasbu0bVlkty5o/edit

----- Como ejecutar ------
1. Correr directamente en Linux o usando una VM en la carpeta del TP: ./deploy.sh install
2. Modificar usando vim/nano los archivos de configuracion (IPs obligatorio si es en PCs distintas y path de swap en memoria)
2. En cada modulo ejecutar: make clean y luego make all
3. Levantar cada modulo sobre su carpeta con: ./bin/nombreModulo.out archivoConfig.config (Siguiendo el orden: memoria -> cpu -> kernel)
4. Para cada consola ejecutar: ./bin/consola.out archivoConInstrucciones tamañoProceso
