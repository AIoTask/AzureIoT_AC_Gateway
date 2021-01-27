#ifndef CLEAN_ROOM_SIM
#define CLEAN_ROOM_SIM

#include <applibs/log.h>

typedef struct {
	int temperature;	// Store Environment Temperature
	int humidity;		// Store Humidity
	int co2Density;	    // Store CO2 Density
} clean_room_env_param;

// Initially set all element`s value to zero
void cr_parameter_init(clean_room_env_param*);

// Update simulation value update
int cr_set_parameter(clean_room_env_param*, int, int, int);

// Print clean room environment value by Log_Debug
void cr_print_info(clean_room_env_param);

#endif