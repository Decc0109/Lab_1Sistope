#include "funciones.h"

int main(int argc, char const *argv[])
{
    // Parte 1: Carga de datos

    // Verificar que se hayan proporcionado los 8 argumentos necesarios para el codigo, todo esto para su buena ejecucion
    if (argc < 9) {
        printf("El programa requiere al menos 8 argumentos para poder trabajar intente nuevamente.\n");
        return 1;
    }

    //inicializamos las variables que vienen al momento de la entrada de datos, estas variables son las que se van a usar a lo largo de todo el programa, por eso es importante inicializarlas al inicio del codigo
    int radio;
    int umbral;
    const char *nombre_archivo;
    const char *reportes_finales;
    int flag_debug = 0;

    //Esta toma de datos se hace de esta forma en vez de asumir las entradas debido a que no sabemos que pasara en caso de que el orden de los argumentos no sea el mismo que el del enunciado,
    //por eso es importante hacer esta toma de datos de esta forma, para asegurar que el programa funcione correctamente sin importar el orden de los argumentos
    for (int i = 1; i < argc; i++) {
        // usaremos strcmp (stringCompare) para comparar los parametros de entrada, debido a que al ser 2 caracteres es mejor usar un comparador de strings y devuelve 0 cuando los textos son idénticos
        if (strcmp(argv[i], "-i") == 0) {
            nombre_archivo = argv[++i];
        }
        else if (strcmp(argv[i], "-r") == 0) {
            radio = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-t") == 0) {
            umbral = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-o") == 0) {
            reportes_finales = argv[++i];
        }
        else if (strcmp(argv[i], "-d") == 0) {
            flag_debug = 1;
        }
    }

    int ancho, alto;
    unsigned char *imagen_original = cargar_imagen(nombre_archivo, &ancho, &alto);
    if (imagen_original == NULL) {
        return 1;
    }

    // Parte 2: Preprocesamiento de datos

    unsigned char *imagen_erosionada = erosionar(imagen_original, ancho, alto);
    if (imagen_erosionada == NULL) {
        free(imagen_original);
        return 1;
    }

    unsigned char *imagen_preprocesada = dilatar(imagen_erosionada, ancho, alto);
    if (imagen_preprocesada == NULL) {
        free(imagen_original);
        free(imagen_erosionada);
        return 1;
    }

    //Fase 3: Analisis de ruido como bien dice el enunciado este dato lo obtenemos restando la imagen original de la preprocesada
    unsigned char *analisis_ruido = calcular_ruido(imagen_original, imagen_preprocesada, ancho, alto);
    if (analisis_ruido == NULL) {
        free(imagen_original);
        free(imagen_erosionada);
        free(imagen_preprocesada);
        return 1;
    }

    // Fase 4: Votación de Hough
    int *acumulador_hough = hough_votar(imagen_preprocesada, ancho, alto, radio);
    if (acumulador_hough == NULL) {
        free(imagen_original);
        free(imagen_erosionada);
        free(imagen_preprocesada);
        free(analisis_ruido);
        return 1;
    }

    // Fase 5: Salida de Resultados
    exportar_reporte(acumulador_hough, ancho, alto, umbral, reportes_finales);

    //Esto es para la fase de debug, si el flag_debug es igual a 1, entonces se van a exportar las imagenes preprocesada y de ruido en formato .bin,
    //esto es para que se pueda revisar estas imagenes y verificar que el preprocesamiento se hizo correctamente,
    if (flag_debug == 1) {
        exportar_bin("preprocesada.bin", imagen_preprocesada, ancho, alto);
        //Ahora hacemos el mismo caso pero para la imagen del ruido.
        exportar_bin("ruido.bin", analisis_ruido, ancho, alto);
    }

    //liberamos los datos
    free(imagen_original);
    free(imagen_erosionada);
    free(imagen_preprocesada);
    free(analisis_ruido);
    free(acumulador_hough);
    return 0;
}
