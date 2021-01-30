/*
 *   Please read the disclaimer and the developer board selection section below
 *
 *
 *
 *   DISCLAIMER
 *
 *   The learning_path_libs functions provided in the learning_path_libs folder:
 *
 *	   1. are NOT supported Azure Sphere APIs.
 *	   2. are prefixed with lp_, typedefs are prefixed with LP_
 *	   3. are built from the Azure Sphere SDK Samples at https://github.com/Azure/azure-sphere-samples
 *	   4. are not intended as a substitute for understanding the Azure Sphere SDK Samples.
 *	   5. aim to follow best practices as demonstrated by the Azure Sphere SDK Samples.
 *	   6. are provided as is and as a convenience to aid the Azure Sphere Developer Learning experience.
 *
 *
 *   DEVELOPER BOARD SELECTION
 *
 *   The following developer boards are supported.
 *
 *	   1. AVNET Azure Sphere Starter Kit.
 *     2. AVNET Azure Sphere Starter Kit Revision 2.
 *	   3. Seeed Studio Azure Sphere MT3620 Development Kit aka Reference Design Board or rdb.
 *	   4. Seeed Studio Seeed Studio MT3620 Mini Dev Board.
 *
 *   ENABLE YOUR DEVELOPER BOARD
 *
 *   Each Azure Sphere developer board manufacturer maps pins differently. You need to select the configuration that matches your board.
 *
 *   Follow these steps:
 *
 *	   1. Open CMakeLists.txt.
 *	   2. Uncomment the set command that matches your developer board.
 *	   3. Click File, then Save to save the CMakeLists.txt file which will auto generate the CMake Cache.
 */

 // Hardware definition
#include "hw/azure_sphere_learning_path.h"

// Learning Path Libraries
#include "azure_iot.h"
#include "config.h"
#include "exit_codes.h"
#include "peripheral_gpio.h"
#include "terminate.h"
#include "timer.h"

// System Libraries
#include "applibs_versions.h"
#include <applibs/gpio.h>
#include <applibs/log.h>
#include <applibs/powermanagement.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

// User-defined Library
#include "uart_conn.h"
#include "http_operation.h"

// Hardware specific
#ifdef OEM_AVNET
#include "board.h"
#include "imu_temp_pressure.h"
#include "light_sensor.h"
#endif // OEM_AVNET

// Hardware specific
#ifdef OEM_SEEED_STUDIO
#include "board.h"
#endif // SEEED_STUDIO

#define LP_LOGGING_ENABLED FALSE
#define JSON_MESSAGE_BYTES 256  // Number of bytes to allocate for the JSON telemetry message for IoT Central
#define IOT_PLUG_AND_PLAY_MODEL_ID "dtmi:com:example:azuresphere:labmonitor;1"	// https://docs.microsoft.com/en-us/azure/iot-pnp/overview-iot-plug-and-play
//#define IOT_PLUG_AND_PLAY_MODEL_ID ""
// Forward signatures
static void get_simulation_data(EventLoopTimer* eventLoopTimer);

LP_USER_CONFIG lp_config;


// GPIO Input Peripherals
static LP_GPIO UARTfileLed = {
	.pin = LED1,
	.direction = LP_OUTPUT,
	.initialState = GPIO_Value_Low,
	.invertPin = true,
	.name = "UARTfileLed" };

// Timers
static LP_TIMER getSimulationData = {
	.period = { 5, 0 },
	.name = "getSimulationData",
	.handler = get_simulation_data };


// Initialize Sets
LP_GPIO* peripheralGpioSet[] = { &UARTfileLed };
LP_TIMER* timerSet[] = { &getSimulationData };

// Message templates and property sets
static int uart_fd;

/// <summary>
/// get_simulation_data 
/// </summary>
static void get_simulation_data(EventLoopTimer* eventLoopTimer)
{

	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0)
	{
		lp_gpioStateSet(&UARTfileLed, false);
		lp_terminate(ExitCode_ConsumeEventLoopTimeEvent);
	}
	else {
		// Log_Debug("Get ENV data...\n");
		http_get_env_data(1, 0, 0);
		// Log_Debug("Transfer AC data...\n");
		http_transfer_ac_data(uart_fd, 1, 0, 0);
	}
}

/// <summary>
///  Initialize peripherals, device twins, direct methods, timers.
/// </summary>
static void InitPeripheralsAndHandlers(void)
{
	// Azure IoT initialize
	lp_azureInitialize(lp_config.scopeId, IOT_PLUG_AND_PLAY_MODEL_ID);

	// DevKit initialize
	lp_initializeDevKit();

	// Open the GPIO
	lp_gpioSetOpen(peripheralGpioSet, NELEMS(peripheralGpioSet));

	// Start timer
	lp_timerSetStart(timerSet, NELEMS(timerSet));

    uart_fd = uart_open(57600);

	if (uart_fd > 0) {
		lp_gpioStateSet(&UARTfileLed, true);
	}
}

/// <summary>
///     Close peripherals and handlers.
/// </summary>
static void ClosePeripheralsAndHandlers(void)
{
	Log_Debug("Closing file descriptors\n");

	

	// Stop timers and Azure IoT Service
	lp_timerSetStop(timerSet, NELEMS(timerSet));
	lp_azureToDeviceStop();

	lp_gpioSetClose(peripheralGpioSet, NELEMS(peripheralGpioSet));

	lp_closeDevKit();

	lp_timerEventLoopStop();

	close(uart_fd);
}


int main(int argc, char* argv[])
{

	// Register Linux signal SIGTERM
	lp_registerTerminationHandler();

	// Parse input command arguments and verify the correctness
	lp_configParseCmdLineArguments(argc, argv, &lp_config);
	if (!lp_configValidate(&lp_config)) {
		return lp_getTerminationExitCode();
	}

	// Initialize user-defined Peripheral and Handlers
	InitPeripheralsAndHandlers();

	// Main loop
	while (!lp_isTerminationRequired())
	{
		int result = EventLoop_Run(lp_timerGetEventLoop(), -1, true);
		// Continue if interrupted by signal, e.g. due to breakpoint being set.
		if (result == -1 && errno != EINTR)
		{
			lp_terminate(ExitCode_Main_EventLoopFail);
		}
	}

	// Close user-defined Peripheral and Handlers
	ClosePeripheralsAndHandlers();

	Log_Debug("Application exiting.\n");
	return lp_getTerminationExitCode();
}