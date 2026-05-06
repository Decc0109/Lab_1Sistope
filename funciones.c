#include "funciones.h"

// Entradas: nombre_archivo (ruta al archivo .bin), ancho y alto donde se guardaran las dimensiones leidas del archivo bin
// Salidas: arreglo de bytes con los pixeles de la imagen, o NULL si ocurre algun error
// Descripcion: Abre el archivo binario de entrada, lee la cabecera que contiene el ancho y alto de la imagen,
//              reserva memoria para los pixeles y los carga en memoria para su uso posterior en el programa
unsigned char *cargar_imagen(const char *nombre_archivo, int *ancho, int *alto) {

    // Abrir el archivo .bin para lectura y su uso posterior
    FILE *archivo = fopen(nombre_archivo, "rb");
    if (archivo == NULL) {
        printf("Error al abrir el archivo.\n");
        return NULL;
    }

    //El enunciado nos dice que el inicio del archivo contiene una cabecera simple: dos enteros (int) que
    //representan el ancho y alto, por los que vamos a tomar los datos de esta cabecera para luego poder trabajar con el resto del archivo
    fread(ancho, sizeof(int), 1, archivo);
    fread(alto, sizeof(int), 1, archivo);

    // Reservar memoria para los datos de la imagen
    unsigned char *imagen_original = (unsigned char *)calloc(*ancho * *alto, sizeof(unsigned char));
    if (imagen_original == NULL) {
        printf("Error al reservar memoria.\n");
        fclose(archivo);
        return NULL;
    }

    // Leer los datos de la imagen
    fread(imagen_original, sizeof(unsigned char), *ancho * *alto, archivo);

    //Luego de tomar los datos cerramos el archivo
    fclose(archivo);

    return imagen_original;
}

// Entradas: imagen_original (arreglo de pixeles binarios), ancho y alto de la imagen en pixeles
// Salidas: nuevo arreglo con la imagen erosionada, o NULL si falla la reserva de memoria
// Descripcion: Aplica la operacion de erosion morfologica usando el elemento estructurante en cruz 3x3.
//              Un pixel se mantiene como 1 solo si el pixel central y sus 4 vecinos directos (arriba, abajo, izquierda, derecha) son todos 1,
//              eliminando asi los pixeles aislados y el ruido fino de la imagen original
unsigned char *erosionar(unsigned char *imagen_original, int ancho, int alto) {

    //Inicializamos la imagen erosionada con ceros, para luego poder ir llenandola con los resultados de la erosion, esta imagen erosionada es la que se va a usar para la fase de dilatacion,
    //esta es una parte importante del preprocesamiento de datos, debido a que la erosion es la que nos va a ayudar a eliminar el ruido de la imagen original,
    //y luego la dilatacion es la que nos va a ayudar a recuperar las formas originales de los objetos en la imagen, por eso es importante hacer esta fase de preprocesamiento de datos correctamente
    unsigned char *imagen_erosionada = (unsigned char *)calloc(ancho * alto, sizeof(unsigned char));
    if (imagen_erosionada == NULL) {
        printf("Error al reservar memoria para la imagen erosionada.\n");
        return NULL;
    }

    // Aplicar la erosión a la imagen
    for (int i = 1; i < alto - 1; i++) {
        for (int j = 1; j < ancho - 1; j++) {
            // Erosión: si todos los píxeles vecinos son 1, el píxel actual erosionado también se convierte en 1
            if (imagen_original[i * ancho + j] == 1 &&
                imagen_original[(i - 1) * ancho + j] == 1 &&
                imagen_original[(i + 1) * ancho + j] == 1 &&
                imagen_original[i * ancho + (j - 1)] == 1 &&
                imagen_original[i * ancho + (j + 1)] == 1) {
                imagen_erosionada[i * ancho + j] = 1;
            }
        }
    }

    return imagen_erosionada;
}

// Entradas: imagen_erosionada (resultado de la erosion previa), ancho y alto de la imagen en pixeles
// Salidas: nuevo arreglo con la imagen dilatada (preprocesada final), o NULL si falla la reserva de memoria
// Descripcion: Aplica la operacion de dilatacion morfologica usando el elemento estructurante en cruz 3x3 (SE3x3).
//              Un pixel se activa como 1 si el pixel central o al menos uno de sus 4 vecinos directos es 1,
//              recuperando el grosor de los bordes que pudo haber adelgazado la erosion previa.
//              Junto con la erosion forma la operacion de apertura morfologica (erosion seguida de dilatacion)
unsigned char *dilatar(unsigned char *imagen_erosionada, int ancho, int alto) {

    //Aplicamos ahora la dilatacion a la imagen erosionada, para esto vamos a crear una nueva variable que esta seria el fin de la fase
    // y seria la fase de preprocesamiento de datos, esta nueva variable se llama imagen_preprocesada y es la que se va a llenar con los resultados de la dilatacion
    unsigned char *imagen_preprocesada = (unsigned char *)calloc(ancho * alto, sizeof(unsigned char));
    if (imagen_preprocesada == NULL) {
        printf("Error al reservar memoria para la imagen preprocesada.\n");
        return NULL;
    }

    // Aplicar la Dilatación (sobre la imagen ya erosionada)
    for (int i = 1; i < alto - 1; i++) {
        for (int j = 1; j < ancho - 1; j++) {

            int indice = i * ancho + j;

            // Dilatación: Si el centro O AL MENOS UN vecino es 1, se convierte en 1
            if (imagen_erosionada[indice] == 1 ||
                imagen_erosionada[(i - 1) * ancho + j] == 1 ||
                imagen_erosionada[(i + 1) * ancho + j] == 1 ||
                imagen_erosionada[i * ancho + (j - 1)] == 1 ||
                imagen_erosionada[i * ancho + (j + 1)] == 1) {
                imagen_preprocesada[indice] = 1;
            }
        }
    }

    return imagen_preprocesada;
}

// Entradas: imagen_original (imagen binaria cruda), imagen_preprocesada (imagen tras la apertura morfologica),
//           ancho y alto de la imagen en pixeles
// Salidas: arreglo con la imagen de ruido (diferencia entre original y preprocesada), o NULL si falla la reserva de memoria
// Descripcion: Obtiene el ruido eliminado por la operacion de apertura restando pixel a pixel la imagen preprocesada
//              de la imagen original. Los pixeles que valen 1 en el resultado son los que fueron eliminados como ruido
unsigned char *calcular_ruido(unsigned char *imagen_original, unsigned char *imagen_preprocesada, int ancho, int alto) {

    //Fase 3: Analisis de ruido como bien dice el enunciado este dato lo obtenemos restando la imagen original de la preprocesada
    unsigned char *analisis_ruido = (unsigned char *)calloc(ancho * alto, sizeof(unsigned char));
    if (analisis_ruido == NULL) {
        printf("Error al reservar memoria para el análisis de ruido.\n");
        return NULL;
    }

    for (int i = 0; i < ancho * alto; i++) {
        // Restamos pixel a pixel: si original=1 y preprocesada=0, ese pixel fue ruido eliminado (resultado=1)
        analisis_ruido[i] = imagen_original[i] - imagen_preprocesada[i];
    }

    return analisis_ruido;
}

// Entradas: imagen_preprocesada (imagen limpia tras la apertura morfologica), ancho y alto de la imagen en pixeles,
//           radio r de los circulos a detectar
// Salidas: plano acumulador de votos (arreglo de enteros del mismo tamaño que la imagen), o NULL si falla la reserva de memoria
// Descripcion: Implementa la Transformada de Hough para deteccion de circunferencias de radio fijo.
//              Por cada pixel blanco (borde) de la imagen preprocesada, vota en el plano acumulador
//              para todos los posibles centros (a,b) usando la ecuacion parametrica: a = x - r*cos(theta), b = y - r*sin(theta).
//              Al finalizar, las celdas con mas votos corresponden a los centros reales de los circulos en la imagen
int *hough_votar(unsigned char *imagen_preprocesada, int ancho, int alto, int radio) {

    // Creamos el plano acumulador lleno de ceros.
    // Usamos sizeof(int) porque guardaremos contadores.
    int *acumulador_hough = (int *)calloc(ancho * alto, sizeof(int));
    if (acumulador_hough == NULL) {
        printf("Error al reservar memoria para el acumulador.\n");
        return NULL;
    }

    for (int y = 0; y < alto; y++) {
        for (int x = 0; x < ancho; x++) {

            // ¡Solo votan los píxeles blancos (bordes) de la imagen limpia!
            if (imagen_preprocesada[y * ancho + x] == 1) {

                // Este calculo es fundamental para poder entender el movimiento y la cantidad de pasos que va a tener el acumulador
                // esto se debe a que finalmente si nosostros definimos un numero base seria muy poco ineficiente y tendriamos que calcular
                // una cantidad de pasos que llevarian a un mismo resultado, es decir puede pasar que se calcule para un mismo caso mas de una vez
                // por esto esta formula basada en el radio nos dira el numero de pasos que deberia dar por iteracion para cubrir todo el circulo,
                // esto se debe a que el radio es el que nos va a dar la cantidad de puntos que hay en el circulo,
                //por lo tanto es importante este calculo para poder hacer una buena votacion
                double step = 1.0 / radio;

                // Iteraremos theta de 0 a 2*PI  poruqe es una oscilacion completa del circulo en radianes
                // Usamos un paso pequeño (0.05) para asegurar una buena votación
                for (double theta = 0.0; theta < 2.0 * M_PI; theta += step) {

                    // Aplicamos estrictamente las fórmulas del enunciado
                    // Hacemos un "cast" a (int) porque los índices de arreglos no tienen decimales
                    int a = (int)(x - radio * cos(theta));
                    int b = (int)(y - radio * sin(theta));

                    // ¡REGLA DE ORO! Verificamos que el centro calculado (a, b)
                    // no se salga de los límites de nuestra imagen real
                    if (a >= 0 && a < ancho && b >= 0 && b < alto) {
                        // Metemos el voto en el buzón
                        acumulador_hough[b * ancho + a]++;
                    }
                }
            }
        }
    }

    return acumulador_hough;
}

// Entradas: acumulador_hough (plano acumulador con votos de Hough), ancho y alto de la imagen en pixeles,
//           umbral (minimo de votos para validar un centro), reportes_finales (nombre del archivo .csv de salida)
// Salidas: ninguna, genera el archivo .csv con las coordenadas detectadas
// Descripcion: Recorre el plano acumulador y escribe en el archivo CSV las coordenadas (X,Y) de todos los centros
//              cuyo numero de votos supere o iguale el umbral de confianza dado, siguiendo el formato que exige el enunciado
void exportar_reporte(int *acumulador_hough, int ancho, int alto, int umbral, const char *reportes_finales) {

    //Primero abrimos el archivo.csv para pode hacer su escritura
    FILE *archivo_salida = fopen(reportes_finales, "w");
    if (archivo_salida == NULL) {
        printf("Error al crear el archivo de reporte.\n");
        return;
    }

    //Escribimos la cabecera que exige el enunciado
    fprintf(archivo_salida, "X,Y\n");

    // 3. Recorrer el acumulador
    for (int y = 0; y < alto; y++) {
        for (int x = 0; x < ancho; x++) {

            // Calculamos el indice lineal a partir de las coordenadas 2D para acceder al arreglo plano del acumulador
            int indice = y * ancho + x;

            // Si los votos en esa coordenada superan o igualan el umbral
            if (acumulador_hough[indice] >= umbral) {

                // Escribir la coordenada en formato "x,y" seguido de un salto de línea
                fprintf(archivo_salida, "%d,%d\n", x, y);
            }
        }
    }

    // Cerramos el archivo
    fclose(archivo_salida);
}

// Entradas: nombre (ruta del archivo .bin a crear), datos (arreglo de pixeles a guardar),
//           ancho y alto de la imagen en pixeles
// Salidas: ninguna, genera el archivo .bin en disco
// Descripcion: Guarda una imagen en formato binario crudo escribiendo primero la cabecera con ancho y alto
//              y luego los bytes de los pixeles, siguiendo el mismo formato del archivo de entrada para
void exportar_bin(const char *nombre, unsigned char *datos, int ancho, int alto) {

    // Inicializamos los arcivos para poder hacer la escritura de la imagen procesada y el ruido,
    //en caso de que no exista dicho archivo donde vamos a escribir se va crear al momento de ejecutar el programa
    FILE *archivo = fopen(nombre, "wb");
    if (archivo != NULL) {
        // Escribimos inicialmente los elementos ancho y alto para que sepa el tamaño que posee la imaggen
        fwrite(&ancho, sizeof(int), 1, archivo);
        fwrite(&alto, sizeof(int), 1, archivo);
        // Escribimos todos los datos dentro de la matriz
        fwrite(datos, sizeof(unsigned char), ancho * alto, archivo);
        fclose(archivo);
    }
}
