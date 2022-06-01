#include <protocolo.h>

// ------- Funciones de Cliente y Servidor -------

void eliminar_paquete(Paquete *paquete)
{
  if (paquete != NULL)
  {
    if (paquete->buffer != NULL)
    {
      free(paquete->buffer->stream);
      free(paquete->buffer);
    }
    free(paquete);
  }
}

void eliminar_linea_instruccion(LineaInstruccion *lineaInstruccion)
{
  if (lineaInstruccion != NULL)
  {
    if (lineaInstruccion->identificador != NULL)
    {
      free(lineaInstruccion->identificador);
    }
    free(lineaInstruccion);
  }
}

void eliminar_pcb(Pcb *pcb)
{
  if (pcb != NULL)
  {
    if (pcb->instrucciones != NULL)
    {
      for (int i = 0; i < list_size(pcb->instrucciones); i++)
      {
        eliminar_linea_instruccion(list_get(pcb->instrucciones, i));
      }
      free(pcb->instrucciones);
    }
    free(pcb);
  }
}

// ------- Funciones de Cliente -------

void inicializar_buffer(Paquete *paquete)
{
  paquete->buffer = malloc(sizeof(Buffer));
  paquete->buffer->tamanio = 0;
  paquete->buffer->stream = NULL;
}

Paquete *crear_paquete(CodigoOperacion codigoOperacion)
{
  Paquete *paquete = malloc(sizeof(Paquete));

  paquete->codigoOperacion = codigoOperacion;
  inicializar_buffer(paquete);

  return paquete;
}

void agregar_a_paquete(Paquete *paquete, void *valor, int tamanio)
{
  paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->tamanio + tamanio + sizeof(int));

  memcpy(paquete->buffer->stream + paquete->buffer->tamanio, &tamanio, sizeof(int));
  memcpy(paquete->buffer->stream + paquete->buffer->tamanio + sizeof(int), valor, tamanio);

  paquete->buffer->tamanio += tamanio + sizeof(int);
}

void *serializar_paquete(Paquete *paquete, int bytes)
{
  void *magic = malloc(bytes);
  int desplazamiento = 0;

  memcpy(magic + desplazamiento, &(paquete->codigoOperacion), sizeof(int));
  desplazamiento += sizeof(int);
  memcpy(magic + desplazamiento, &(paquete->buffer->tamanio), sizeof(int));
  desplazamiento += sizeof(int);
  memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->tamanio);

  return magic;
}

void enviar_paquete_a_servidor(Paquete *paquete, int socketCliente)
{
  int bytes = paquete->buffer->tamanio + 2 * sizeof(int);
  void *aEnviar = serializar_paquete(paquete, bytes);

  send(socketCliente, aEnviar, bytes, 0);
  free(aEnviar);
}

void enviar_mensaje_a_servidor(char *mensaje, int socketCliente)
{
  Paquete *paquete = crear_paquete(MENSAJE);

  paquete->buffer->tamanio = strlen(mensaje) + 1;
  paquete->buffer->stream = malloc(paquete->buffer->tamanio);
  memcpy(paquete->buffer->stream, mensaje, paquete->buffer->tamanio);

  enviar_paquete_a_servidor(paquete, socketCliente);
  eliminar_paquete(paquete);
}

// ------- Funciones de Servidor -------

CodigoOperacion obtener_codigo_operacion(int socketCliente)
{
  CodigoOperacion codigoOperacion;

  if (recv(socketCliente, &codigoOperacion, sizeof(int), MSG_WAITALL) > 0)
    return codigoOperacion;
  else
  {
    close(socketCliente);
    return DESCONEXION;
  }
}

void *obtener_buffer_del_cliente(int *tamanio, int socketCliente)
{
  void *buffer;

  recv(socketCliente, tamanio, sizeof(int), MSG_WAITALL);
  buffer = malloc(*tamanio);
  recv(socketCliente, buffer, *tamanio, MSG_WAITALL);

  return buffer;
}

char *obtener_mensaje_del_cliente(int socketCliente)
{
  int tamanio;
  char *mensaje = obtener_buffer_del_cliente(&tamanio, socketCliente);

  return mensaje;
}

Lista *obtener_paquete_como_lista(int socketCliente)
{
  int tamanioBuffer;
  int tamanioContenido;
  int desplazamiento = 0;

  Lista *contenido = list_create();
  void *buffer = obtener_buffer_del_cliente(&tamanioBuffer, socketCliente);

  while (desplazamiento < tamanioBuffer)
  {
    memcpy(&tamanioContenido, buffer + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);

    void *valor = malloc(tamanioContenido);
    memcpy(valor, buffer + desplazamiento, tamanioContenido);
    desplazamiento += tamanioContenido;

    list_add(contenido, valor);
  }

  free(buffer);
  return contenido;
}

void serializar_lista_de_instrucciones(Paquete *paquete, Lista *instrucciones, int tamanioProceso)
{
  int cantidadInstrucciones = list_size(instrucciones);

  agregar_a_paquete(paquete, &tamanioProceso, sizeof(int));
  agregar_a_paquete(paquete, &cantidadInstrucciones, sizeof(int));

  LineaInstruccion *lineaInstruccion;

  for (int i = 0; i < cantidadInstrucciones; i++)
  {
    lineaInstruccion = list_get(instrucciones, i);

    agregar_a_paquete(paquete, lineaInstruccion->identificador, strlen(lineaInstruccion->identificador) + 1);
    agregar_a_paquete(paquete, &(lineaInstruccion->parametros[0]), sizeof(int));
    agregar_a_paquete(paquete, &(lineaInstruccion->parametros[1]), sizeof(int));
  }

  eliminar_linea_instruccion(lineaInstruccion);
}

void deserializar_lista_de_instrucciones(Lista *listaInstrucciones, Lista *listaPlana, int indiceTamanio, int indiceLista)
{
  int cantidadInstrucciones = *(int *)list_get(listaPlana, indiceTamanio);

  for (int i = 0; i < cantidadInstrucciones; i++)
  {
    LineaInstruccion *lineaInstruccion = malloc(sizeof(LineaInstruccion));
    short base = 3 * i;

    lineaInstruccion->identificador = list_get(listaPlana, base + indiceLista + 1);
    lineaInstruccion->parametros[0] = *(int *)list_get(listaPlana, base + indiceLista + 2);
    lineaInstruccion->parametros[1] = *(int *)list_get(listaPlana, base + indiceLista + 3);

    list_add(listaInstrucciones, lineaInstruccion);
  }
}

void serializar_pcb(Paquete *paquete, Pcb *pcb)
{
  int cantidadInstrucciones = list_size(pcb->instrucciones);

  agregar_a_paquete(paquete, &(pcb->pid), sizeof(int));
  agregar_a_paquete(paquete, &(pcb->tamanio), sizeof(int));
  agregar_a_paquete(paquete, &(pcb->contadorPrograma), sizeof(int));
  agregar_a_paquete(paquete, &(pcb->tablaPaginas), sizeof(int));
  agregar_a_paquete(paquete, &(pcb->estimacionRafaga), sizeof(float));
  agregar_a_paquete(paquete, &(pcb->escenario->estado), sizeof(Estado));
  agregar_a_paquete(paquete, &(pcb->escenario->tiempoBloqueadoIO), sizeof(int));
  agregar_a_paquete(paquete, &cantidadInstrucciones, sizeof(int));

  serializar_lista_de_instrucciones(paquete, pcb->instrucciones, pcb->tamanio);
}

Pcb *deserializar_pcb(int socketCliente)
{
  Lista *propiedadesPlanas = obtener_paquete_como_lista(socketCliente);
  Lista *listaResultado = list_create();
  Pcb *pcb = malloc(sizeof(Pcb));

  pcb->pid = *(int *)list_get(propiedadesPlanas, 0);
  pcb->tamanio = *(int *)list_get(propiedadesPlanas, 1);
  pcb->contadorPrograma = *(int *)list_get(propiedadesPlanas, 2);
  pcb->tablaPaginas = *(int *)list_get(propiedadesPlanas, 3);
  pcb->estimacionRafaga = *(float *)list_get(propiedadesPlanas, 4);
  pcb->escenario = malloc(sizeof(Escenario));
  pcb->escenario->estado = *(Estado *)list_get(propiedadesPlanas, 5);
  pcb->escenario->tiempoBloqueadoIO = *(int *)list_get(propiedadesPlanas, 6);

  deserializar_lista_de_instrucciones(listaResultado, propiedadesPlanas, 7, 9);

  pcb->instrucciones = list_duplicate(listaResultado);

  list_destroy(propiedadesPlanas);
  list_destroy(listaResultado);

  return pcb;
}

void enviar_paquete_a_cliente(Paquete *paquete, int socketCliente)
{
  enviar_paquete_a_servidor(paquete, socketCliente);
}