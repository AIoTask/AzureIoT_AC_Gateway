#include "uart_conn.h"

// While return value is positive, it means the file descriptor of UART
// Otherwise, it means the error number of opening UART
int uart_open(unsigned int baudrate) {
	// UART Testing
	UART_Config uart_conf;
	UART_InitConfig(&uart_conf);
	
	uart_conf.baudRate = baudrate;

	int fd = UART_Open(UART0, &uart_conf);

	if (fd == -1) {
		Log_Debug("Error During Open [%d]...\n", errno);
		return -errno;
	}

	Log_Debug("Successfully open UART, file descriptor No. [%d], Baudrate: %d...\n", fd, baudrate);

	return fd;
}