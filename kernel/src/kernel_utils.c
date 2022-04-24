#include <kernel_utils.h>

void cargar_configuracion(void)
{
    t_config *config = config_create("kernel.config");
    if (config == NULL)
    {
        perror("archivo kernel.config NO ENCONTRADO");
        return;
    }
    valores_config.ip_kernel = config_get_string_value(config, "IP_KERNEL");
    valores_config.ip_cpu = config_get_string_value(config, "IP_CPU");
    valores_config.ip_memoria = config_get_string_value(config, "IP_MEMORIA");
    valores_config.puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
    valores_config.puerto_cpu_dispatch = config_get_string_value(config, "PUERTO_CPU_DISPATCH");
    valores_config.puerto_cpu_interrupt = config_get_string_value(config, "PUERTO_CPU_INTERRUPT");
    valores_config.puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
    valores_config.algoritmo_planificacion = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
    valores_config.estimacion_inicial = config_get_int_value(config, "ESTIMACION_INICIAL");
    valores_config.alfa = config_get_double_value(config, "ALFA");
    valores_config.grado_multiprogramacion = config_get_int_value(config, "GRADO_MULTIPROGRAMACION");
    valores_config.tiempo_maximo_bloqueado = config_get_int_value(config, "TIEMPO_MAXIMO_BLOQUEADO");
    // config_destroy(config);
}
int iniciar_servidor_kernel(t_log *logger)
{
    int socketKernel = iniciar_servidor(valores_config.ip_kernel, valores_config.puerto_escucha);
    log_info(logger, "Módulo Kernel listo para recibir el Módulo Consola");
    return socketKernel;
}

int obtener_socket_consola(int socketKernel, t_log *logger)
{
    int socketCliente = esperar_cliente(socketKernel);
    log_info(logger, "Se conectó el Módulo de Consola...");

    return socketCliente;
}

void apagar_servidor_kernel(int socketKernel, t_log *logger)
{
    apagar_servidor(socketKernel);
    log_error(logger, "La Consola se desconectó. Apagando Servidor Kernel.");
}

int crear_conexion_cpu_dispatch(void)
{
    return crear_conexion_con_servidor(valores_config.ip_cpu, valores_config.puerto_cpu_dispatch);
}

int crear_conexion_cpu_interrupt(void)
{
    return crear_conexion_con_servidor(valores_config.ip_cpu, valores_config.puerto_cpu_interrupt);
}

void liberar_conexion_con_cpu(int socketKernel)
{
    liberar_conexion_con_servidor(socketKernel);
}

int crear_conexion_con_memoria(void)
{
    return crear_conexion_con_servidor(valores_config.ip_memoria, valores_config.puerto_memoria);
}

void *recibir_mensajes(int socketCliente)
{

    t_log *logger = log_create("Kernel.log", "Kernel", true, LOG_LEVEL_DEBUG);

    char *mensaje;
    t_list *listaInstrucciones;

    while (true)
    {
        cod_op_servidor codOp = obtener_codigo_operacion(socketCliente);
        switch (codOp)
        {
        case MENSAJE_CLIENTE:
            mensaje = obtener_mensaje(socketCliente);
            log_info(logger, "Recibí el mensaje: %s", mensaje);
            break;
        case 10: //(10-INICIAR_PROCESO)
            listaInstrucciones = list_create();

            recibir_lista_intrucciones(socketCliente, listaInstrucciones);

            log_info(logger, "Recibi INICIAR_PROCESO y  la lista de instrucciones");

            iniciar_proceso(listaInstrucciones);

            break;
        case DESCONEXION_CLIENTE:
            log_info(logger, "Se DESCONECTO un cliente");
            break;
        default:
            log_warning(logger, "Operación desconocida.");
            break;
        }
    }
    log_destroy(logger);
}
void *iniciar_escucha(int socketServidor)
{
    while (1)
    {
        int socketCliente = esperar_cliente(socketServidor);
        pthread_t hiloCliente;

        pthread_create(&hiloCliente, NULL, (void *)recibir_mensajes, (void *)socketCliente);
    }
}

void conectar_cpu(void)
{
    int socketKernelCliente = crear_conexion_cpu_dispatch();

    enviar_mensaje("soy kernel, envio un mensaje al modulo CPU", socketKernelCliente);

    liberar_conexion_con_cpu(socketKernelCliente);
}

void conectar_memoria(void)
{
    int socketKernelCliente = crear_conexion_con_memoria();

    enviar_mensaje("soy kernel, envio un mensaje al modulo Memoria", socketKernelCliente);

    liberar_conexion_con_servidor(socketKernelCliente);
}