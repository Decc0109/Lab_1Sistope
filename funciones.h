#ifndef FUNCIONES_H
#define FUNCIONES_H
#endif

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>


#define M_PI 3.14159265358979323846

unsigned char *cargar_imagen(const char *nombre_archivo, int *ancho, int *alto);
unsigned char *erosionar(unsigned char *imagen_original, int ancho, int alto);
unsigned char *dilatar(unsigned char *imagen_erosionada, int ancho, int alto);
unsigned char *calcular_ruido(unsigned char *imagen_original, unsigned char *imagen_preprocesada, int ancho, int alto);
int *hough_votar(unsigned char *imagen_preprocesada, int ancho, int alto, int radio);
void exportar_reporte(int *acumulador_hough, int ancho, int alto, int umbral, const char *reportes_finales);
void exportar_bin(const char *nombre, unsigned char *datos, int ancho, int alto);


