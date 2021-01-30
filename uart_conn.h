#ifndef UART_CONN
#define UART_CONN

#define UART_STRUCTS_VERSION 1

#include <errno.h>

#include <applibs/uart.h>
#include <applibs/log.h>
// Hardware definition
#include "hw/azure_sphere_learning_path.h"

int uart_open(unsigned int);

#endif