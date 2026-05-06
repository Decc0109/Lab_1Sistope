#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#define M_PI 3.14159265358979323846

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
    char* nombre_archivo;
    char* reportes_finales;
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

    // Abrir el archivo .bin para lectura y su uso posterior
    FILE *archivo;
    archivo = fopen(nombre_archivo, "rb");
    if (archivo == NULL) {
        printf("Error al abrir el archivo.\n");
        return 1;
    }

    //El enunciado nos dice que el inicio del archivo contiene una cabecera simple: dos enteros (int) que 
    //representan el ancho y alto, por los que vamos a tomar los datos de esta cabecera para luego poder trabajar con el resto del archivo
    int ancho, alto;
    fread(&ancho, sizeof(int), 1, archivo);
    fread(&alto, sizeof(int), 1, archivo);

    // Reservar memoria para los datos de la imagen
    unsigned char *imagen_original = (unsigned char *)malloc(ancho * alto * sizeof(unsigned char)); 
    if (imagen_original == NULL) {
        printf("Error al reservar memoria.\n");
        fclose(archivo);
        return 1;
    }

    // Leer los datos de la imagen 
    fread(imagen_original, sizeof(unsigned char), ancho * alto, archivo);


    //Luego de tomar los datos cerramos el archivo
    fclose(archivo);


    // Parte 2: Preprocesamiento de datos



    //Inicializamos la imagen erosionada con ceros, para luego poder ir llenandola con los resultados de la erosion, esta imagen erosionada es la que se va a usar para la fase de dilatacion, 
    //esta es una parte importante del preprocesamiento de datos, debido a que la erosion es la que nos va a ayudar a eliminar el ruido de la imagen original, 
    //y luego la dilatacion es la que nos va a ayudar a recuperar las formas originales de los objetos en la imagen, por eso es importante hacer esta fase de preprocesamiento de datos correctamente
    unsigned char *imagen_erosionada = (unsigned char *)malloc(ancho * alto * sizeof(unsigned char));
    if (imagen_erosionada == NULL) {
        printf("Error al reservar memoria para la imagen erosionada.\n");
        free(imagen_original);
        return 1;
    }

    


    for (int i = 0; i < ancho * alto; i++) {
        imagen_erosionada[i] = 0;
    }

    // Aplicar la erosión a la imagen
    for (int i = 1; i < alto - 1; i++) {
        for (int j = 1; j < ancho - 1; j++) {
            // Erosión: si todos los píxeles vecinos son 1, el píxel actual erosionado también se convierte en 1
            if (imagen_original[i * ancho + j] == 1 &&
                imagen_original[(i - 1) * ancho + j] == 1 &&
                imagen_original[(i + 1) * ancho + j] == 1 &&
                imagen_original[i * ancho + (j - 1)] == 1 &&
                imagen_original[i * ancho + (j + 1)] == 1 ) {
                    imagen_erosionada[i * ancho + j] = 1;
            }
        }
    }

    //Aplicamos ahora la dilatacion a la imagen erosionada, para esto vamos a crear una nueva variable que esta seria el fin de la fase 
    // y seria la fase de preprocesamiento de datos, esta nueva variable se llama imagen_preprocesada y es la que se va a llenar con los resultados de la dilatacion
    unsigned char *imagen_preprocesada = (unsigned char *)malloc(ancho * alto * sizeof(unsigned char));
    if (imagen_preprocesada == NULL) {
        printf("Error al reservar memoria para la imagen preprocesada.\n");
        free(imagen_original);
        free(imagen_erosionada);
        return 1;
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
                imagen_erosionada[i * ancho + (j + 1)] == 1 ) {
                
                imagen_preprocesada[indice] = 1;
            }
        }
    }


    //Fase 3: Analisis de ruido como bien dice el enunciado este dato lo obtenemos restando la imagen original de la preprocesada
    unsigned char *analisis_ruido = (unsigned char *)malloc(ancho * alto * sizeof(unsigned char));
    if (analisis_ruido == NULL) {
        printf("Error al reservar memoria para el análisis de ruido.\n");
        free(imagen_original);
        free(analisis_ruido);
        return 1;
    }

    for (int i = 0; i < ancho * alto; i++) {
        analisis_ruido[i] = imagen_original[i] - imagen_preprocesada[i];
    }

    // Fase 4: Votación de Hough

    // Creamos el plano acumulador lleno de ceros.
    // Usamos sizeof(int) porque guardaremos contadores.
    int *acumulador_hough = (int *)malloc(ancho * alto * sizeof(int));
    if (acumulador_hough == NULL) {
        printf("Error al reservar memoria para el acumulador.\n");
        return 1;
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


    // Fase 5: Salida de Resultados

    //Primero abrimos el archivo.csv para pode hacer su escritura
    FILE *archivo_salida = fopen(reportes_finales, "w");
    if (archivo_salida == NULL) {
        printf("Error al crear el archivo de reporte.\n");
        free(imagen_original);
        free(imagen_erosionada);
        free(imagen_preprocesada);
        free(analisis_ruido);
        free(acumulador_hough);
        return 1;
    }

    //Escribimos la cabecera que exige el enunciado
    fprintf(archivo_salida, "X,Y\n");

    // 3. Recorrer el acumulador
    for (int y = 0; y < alto; y++) {
        for (int x = 0; x < ancho; x++) {
            
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


    //Esto es para la fase de debug, si el flag_debug es igual a 1, entonces se van a exportar las imagenes preprocesada y de ruido en formato .bin, 
    //esto es para que se pueda revisar estas imagenes y verificar que el preprocesamiento se hizo correctamente,
    if (flag_debug == 1) {
        
        // Inicializamos los  arcivos para poder hacer la escritura de la imagen procesada y el ruido, 
        //en caso de que no exista dicho archivo donde vamos a escribir se va crear al momento de ejecutar el programa
        FILE *archivo_prep = fopen("preprocesada.bin", "wb");
        if (archivo_prep != NULL) {
            // Escribimos inicialmente los elementos ancho y alto para que sepa el tamaño que posee la imaggen
            fwrite(&ancho, sizeof(int), 1, archivo_prep);
            fwrite(&alto, sizeof(int), 1, archivo_prep);
            // Escribimos todos los datos dentro de la matriz
            fwrite(imagen_preprocesada, sizeof(unsigned char), ancho * alto, archivo_prep);
            fclose(archivo_prep);
        }

        //Ahora hacemos el mismo caso pero para la imagen del ruido.
        FILE *archivo_ruido = fopen("ruido.bin", "wb");
        if (archivo_ruido != NULL) {
            fwrite(&ancho, sizeof(int), 1, archivo_ruido);
            fwrite(&alto, sizeof(int), 1, archivo_ruido);
            fwrite(analisis_ruido, sizeof(unsigned char), ancho * alto, archivo_ruido);
            fclose(archivo_ruido);

            
        }
    }



    //liberamos los datos

    free(imagen_original);
    free(imagen_erosionada);
    free(imagen_preprocesada);
    free(analisis_ruido);
    free(acumulador_hough);
    return 0;
}

