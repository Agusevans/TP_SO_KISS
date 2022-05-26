#ifndef KERNEL_UTILS_H_
#define KERNEL_UTILS_H_

#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/log.h>
#include <socket/servidor.h>
#include <socket/cliente.h>
#include <socket/protocolo.h>
#include <pthread.h>
#include <proceso.h>

typedef struct
{
    char *IP_KERNEL;
    char *IP_CPU;
    char *IP_MEMORIA;
    char *PUERTO_MEMORIA;
    char *PUERTO_CPU_DISPATCH;
    char *PUERTO_CPU_INTERRUPT;
    char *PUERTO_ESCUCHA;
    char *ALGORITMO_PLANIFICACION;
    int ESTIMACION_INICIAL;
    double ALFA;
    int GRADO_MULTIPROGRAMACION;
    int TIEMPO_MAXIMO_BLOQUEADO;

} kernel_config;

kernel_config valoresConfig;

t_log *logger;

int socketKernelClienteDispatch;

int id_proceso_total;

t_paquete *serializar_pcb(pcb *proceso);

t_list *deserializar_lineas_codigo(t_list *);

/**
 * @brief Carga datos del archivo config en un struct valores_config.
 *
 */
void cargar_configuracion(void);

/**
 * @brief Iniciar el servidor del módulo Kernel para conectar el Módulo de Consola (como cliente).
 *
 * @param logger Logger de Kernel.
 *
 * @return Socket del servidor Kernel (int).
 */
int iniciar_servidor_kernel(t_log *logger);

/**
 * @brief Obtener el socket del Módulo de Consola.
 *
 * @param socketKernel Socket del servidor Kernel (int).
 * @param logger Logger de Kernel.
 *
 * @return Socket de Consola (int).
 */
int obtener_socket_consola(int socketKernel, t_log *logger);

/**
 * @brief Apagar el servidor de Kernel.
 *
 * @param socketKernel Socket del servidor Kernel (int).
 * @param logger Logger de Kernel.
 */
void apagar_servidor_kernel(int socketKernel, t_log *logger);

/**
 * @brief Conectar Kernel con el servidor de CPU a través del puerto Dispatch.
 *
 * @return Socket de Kernel.
 */
int crear_conexion_con_cpu_dispatch(void);

/**
 * @brief Conectar Kernel con el servidor de CPU a través del puerto Interrupt.
 *
 * @return Socket de Kernel.
 */
int crear_conexion_con_cpu_interrupt(void);

/**
 * @brief Liberar la conexión con el servidor del CPU.
 *
 * @param socketKernel Socket del Kernel (int).
 */
void liberar_conexion_con_cpu(int socketKernel);

/**
 * @brief Conectar Kernel con el servidor de Memoria.
 *
 * @return Socket de Kernel.
 */
int crear_conexion_con_memoria(void);

/**
 * @brief escucha mensajes de un determinado socket.
 *
 * @param socket Socket del cual recibe mensajes (int).
 */
void recibir_mensajes(int);

/**
 * @brief escucha peticiones de conexion.
 *
 * @param socketServidor Socket del servidor para aceptar clientes (int).
 */
void *iniciar_escucha(int);

/**
 * @brief conexion con cpu
 *
 */
void conectar_cpu(void);

/**
 * @brief conexion con memoria
 *
 */
void conectar_memoria(void);

char *leer_cola(t_queue *cola);

void *queue_peek_at(t_queue *self, int index);

void agregar_lista_a_paquete(t_paquete *paquete, pcb *proceso);

void enviar_pcb(pcb *, int, t_log *);

void recibir_imagen_pcb(int);

#endif
