#include "clean_room_sim.h"
#include <applibs/log.h>

void cr_parameter_init(clean_room_env_param* crep) {
	Log_Debug("Initialize clean room \n");
	crep->temperature = 0;
	crep->humidity = 0;
	crep->co2Density = 0;
}

int cr_set_parameter(clean_room_env_param* crep, int new_temp, int new_humid, int new_co2) {
	Log_Debug("Set clean room parameter. Temperature = %d, Humidity = %d, CO2 Density = %d\n", new_temp, new_humid, new_co2);
	crep->temperature = new_temp;
	crep->humidity = new_humid;
	crep->co2Density = new_co2;
	return 0;
}

void cr_print_info(clean_room_env_param crep) {
	Log_Debug("Set clean room parameter. Temperature = %d, Humidity = %d, CO2 Density = %d\n",
														crep.temperature, crep.humidity, crep.co2Density);
}