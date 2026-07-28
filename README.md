LABORATORIO 1 - SISTEMAS OPERATIVOS
==================================

Repositorio oficial del Laboratorio 1 de Sistemas Operativos.

Este proyecto fue desarrollado en lenguaje C utilizando GCC y Makefile.

REQUISITOS

Antes de comenzar, debe tener instalado:

- Git
- GCC
- Make

INSTALACION EN UBUNTU / LINUX MINT
----------------------------------

Abrir una terminal y ejecutar:

sudo apt update
sudo apt install build-essential git

TUTORIAL DE CLONACION DEL REPOSITORIO

1. Abrir una terminal

Puede utilizar:
- Terminal de Linux
- WSL
- Git Bash
- Terminal integrada de VSCode

--------------------------------------------------

2. Ir a la carpeta donde desea guardar el proyecto

Ejemplo:

cd ~/projects

Si la carpeta no existe:

mkdir ~/projects
cd ~/projects

--------------------------------------------------

3. Clonar el repositorio

Ejecutar el siguiente comando:

git clone https://github.com/Decc0109/Lab_1Sistope.git

Esto descargará todos los archivos del proyecto.

--------------------------------------------------

4. Entrar a la carpeta del proyecto

Ejecutar:

cd Lab_1Sistope

--------------------------------------------------

COMPILACION DEL PROGRAMA
------------------------

Para compilar el laboratorio se utiliza el archivo Makefile incluido en el proyecto.

Ejecutar:

make

Si la compilacion fue correcta, se generara el ejecutable:

./lab1

Para limpiar los archivos generados por la compilacion:

make clean

--------------------------------------------------

EJECUCION DEL PROGRAMA
----------------------

El programa recibe los parametros mediante flags por consola.

Formato general:

./lab1 -i archivo_entrada.bin -r radio -t umbral -o archivo_salida.csv

Donde:

- -i: Ruta del archivo de imagen binaria de entrada.
- -r: Radio de los circulos que se desean detectar.
- -t: Umbral minimo de votos para considerar un centro como valido.
- -o: Nombre del archivo CSV donde se guardaran los centros detectados.
- -d: Flag opcional para generar archivos de depuracion.

Ejemplo:

./lab1 -i imagen_ruido.bin -r 60 -t 70 -o reporte.csv

--------------------------------------------------

EJEMPLOS DE EJECUCION
---------------------

Ejemplo 1: deteccion con radio 60 y umbral 70

./lab1 -i imagen_ruido.bin -r 60 -t 70 -o reporte.csv

Ejemplo 2: deteccion con radio 40 y umbral 50

./lab1 -i imagen_ruido.bin -r 40 -t 50 -o salida.csv

Ejemplo 3: deteccion con radio 80 y umbral 90

./lab1 -i imagen_ruido.bin -r 80 -t 90 -o salida_radio80.csv

--------------------------------------------------

MODO DEBUG
----------

Si se agrega la flag -d, el programa genera archivos binarios adicionales para revisar las etapas intermedias del procesamiento.

Ejemplo:

./lab1 -i imagen_ruido.bin -r 60 -t 70 -o reporte.csv -d

Archivos generados:

- preprocesada.bin: Imagen despues de aplicar erosion y dilatacion.
- ruido.bin: Imagen con el ruido eliminado durante el preprocesamiento.

--------------------------------------------------

ARCHIVO DE SALIDA
-----------------

El archivo de salida se guarda en formato CSV.

La primera linea contiene la cabecera:

X,Y

Luego se escriben las coordenadas de los centros detectados.

Ejemplo:

X,Y
752,241
675,188
662,270

--------------------------------------------------

FORMATO DEL ARCHIVO BINARIO
---------------------------

El archivo de entrada debe tener el siguiente formato:

1. Ancho de la imagen como int de 4 bytes.
2. Alto de la imagen como int de 4 bytes.
3. Pixeles de la imagen, donde cada pixel es un byte con valor 0 o 1.

El total de pixeles debe ser:

ancho * alto

--------------------------------------------------

ESTRUCTURA DEL PROYECTO
-----------------------

Archivos principales:

- lab1.c: Contiene la funcion main y el flujo general del programa.
- funciones.c: Contiene la implementacion de las funciones utilizadas.
- funciones.h: Contiene las cabeceras de las funciones.
- Makefile: Archivo utilizado para compilar el programa.
- README.md: Documento con instrucciones de instalacion, compilacion y ejecucion.
- reporte.csv: Archivo de ejemplo con centros detectados.

--------------------------------------------------

DESCRIPCION GENERAL DEL PROCESO
-------------------------------

El programa realiza las siguientes etapas:

1. Carga la imagen binaria desde el archivo indicado con -i.
2. Aplica erosion para eliminar ruido.
3. Aplica dilatacion para recuperar la forma despues de la erosion.
4. Calcula la imagen de ruido eliminado.
5. Aplica la Transformada de Hough para detectar centros de circulos con el radio indicado.
6. Guarda en un archivo CSV los centros que superan el umbral indicado.

--------------------------------------------------
