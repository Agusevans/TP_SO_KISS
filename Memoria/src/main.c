#include <memoria_utils.h>

int main(void)
{
  Config *config = config_create("Memoria.config");
  Logger *logger = iniciar_logger_memoria();

  rellenar_config_memoria(config);

  log_info(logger, "Iniciando Servidor Memoria...");
  int socketMemoria = iniciar_servidor_memoria();

  if (socketMemoria < 0)
  {
    log_error(logger, "Error intentando iniciar Servidor Memoria.");
    return EXIT_FAILURE;
  }

  log_info(logger, "Servidor Memoria iniciado correctamente.");

  Hilo hiloCliente;

  while (true)
  {
    int socketCliente = esperar_cliente(socketMemoria);

    pthread_create(&hiloCliente, NULL, (void *)manejar_paquetes_clientes, (void *)socketCliente);
    pthread_join(hiloCliente, NULL);
  }

  return EXIT_SUCCESS;
}