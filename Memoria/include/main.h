#ifndef MAIN_H
#define MAIN_H

typedef struct Pagina Pagina;

typedef struct
{
    int idProceso;
    int numeroMarco;
    Pagina *paginaActual; // NULL = vacio
} Marco;

struct Pagina
{
    int numeroPagina;
    bool paginaVacia;
    bool uso;
    bool modificado;
    Marco *marcoAsignado;
};

typedef struct
{
    t_list *entradas; // puntero a paginas
    int numeroTablaSegundoNivel;
} TablaSegundoNivel;

typedef struct
{
    t_list *entradas; // cada entrada es puntero a tabla 2 nivel
    int numeroTablaPrimerNivel;
} TablaPrimerNivel;

typedef struct
{
    int idProceso;
    int tamanio;
    TablaPrimerNivel *tablaPrimerNivel;
    FILE *archivoSwap;
    t_list *paginasAsignadas;
    int posicionDelPunteroDeSustitucion;
} Proceso;

#endif