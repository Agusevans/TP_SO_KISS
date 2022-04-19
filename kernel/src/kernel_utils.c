#include <kernel_utils.h>

int iniciar_servidor_kernel(t_log *logger)
{
	int socketKernel = iniciar_servidor("127.0.0.1", "5000");
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

int crear_conexion_con_cpu(void)
{
	return crear_conexion_con_servidor("127.0.0.1", "5001");
}

void liberar_conexion_con_cpu(int socketKernel)
{
	liberar_conexion_con_servidor(socketKernel);
}