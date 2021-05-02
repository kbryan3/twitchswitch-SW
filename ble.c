/*
 * ble.c
 *
 *  This file contains the function associated with ble including the
 *     main ble state machine to handle ble events
 *
 *  @author Kyle Bryan
 *  @date October 2020
 *  @resources switch statement based on the Simplicity SOC Thermometer example
 */

#include "ble.h"



static uint8_t indicate_Temp_Flag = 0;
void handle_ble_event(struct gecko_cmd_packet* evt)
{
	/* Flag for indicating DFU Reset must be performed */
	static uint8_t boot_to_dfu = 0;
	static uint8_t connection;
	uint8_t *server_BLE_Address;
	int8_t rssi;
	switch (BGLIB_MSG_ID(evt->header)) {
	      /* This boot event is generated when the system boots up after reset.
	       * Do not call any stack commands before receiving the boot event.
	       * Here the system is set to start advertising immediately after boot procedure. */
	      case gecko_evt_system_boot_id:
	    	/* Set advertising parameters. 100ms advertisement interval.
	         * The first two parameters are minimum and maximum advertising interval, both in
	         * units of (milliseconds * 1.6). */
	    	BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_set_advertise_timing(0, 400, 400, 0, 0));

	    	gecko_cmd_system_set_tx_power(50);

	        /* Start general advertising and enable connections. */
	    	BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_start_advertising(0, le_gap_general_discoverable, le_gap_connectable_scannable));

	        break;

	      case gecko_evt_le_connection_parameters_id:
	    	  LOG_INFO("Connection Parameters");
	    	  LOG_INFO("Interval Time: %d ms", ((evt->data.evt_le_connection_parameters.interval) *100/80));
	    	  LOG_INFO("Connection Intervals: %d", evt->data.evt_le_connection_parameters.latency);
	    	  LOG_INFO("Timeout Time: %d ms", (evt->data.evt_le_connection_parameters.timeout)*10);
	    	  BTSTACK_CHECK_RESPONSE(gecko_cmd_le_connection_get_rssi(connection));

	    	  break;

	      /* This event is generated when a connected client has either
	       * 1) changed a Characteristic Client Configuration, meaning that they have enabled
	       * or disabled Notifications or Indications, or
	       * 2) sent a confirmation upon a successful reception of the indication. */
	      case gecko_evt_gatt_server_characteristic_status_id:
	        /* Check that the characteristic in question is temperature - its ID is defined
	         * in gatt.xml as "temperature_measurement". Also check that status_flags = 1, meaning that
	         * the characteristic client configuration was changed (notifications or indications
	         * enabled or disabled). */
	        if ((evt->data.evt_gatt_server_characteristic_status.characteristic == gattdb_accel_read)
	            && (evt->data.evt_gatt_server_characteristic_status.status_flags == 0x01)) {
	          if (evt->data.evt_gatt_server_characteristic_status.client_config_flags == 0x02) {

	            /* Indications have been turned ON - start the repeating timer. The 1st parameter '32768'
	             * tells the timer to run for 1 second (32.768 kHz oscillator), the 2nd parameter is
	             * the timer handle and the 3rd parameter '0' tells the timer to repeat continuously until
	             * stopped manually.*/
	            gecko_cmd_hardware_set_soft_timer(32768, 0, 0);
	        	indicate_Temp_Flag = 1;
	          } else if (evt->data.evt_gatt_server_characteristic_status.client_config_flags == 0x00) {
	            /* Indications have been turned OFF - stop the timer. */
	            gecko_cmd_hardware_set_soft_timer(0, 0, 0);
	        	indicate_Temp_Flag = 0;
	          }
	        }
	        break;
	      case gecko_evt_le_connection_rssi_id:
	    	  //response to a gecko_cmd_le_connection_get_rssi command
	    	  rssi = evt->data.evt_le_connection_rssi.rssi;
	    	  BTSTACK_CHECK_RESPONSE(gecko_cmd_system_halt(1));
	    	  if(rssi > -35 )
	    	  {
	    		  gecko_cmd_system_set_tx_power(TX_POWER_MIN);
	    		  LOG_INFO("RSSI: %d, Power: %d", rssi, TX_POWER_MIN);
	    	  }
	    	  else if(rssi <= -35 && rssi > -45)
	    	  {
	    		  gecko_cmd_system_set_tx_power(-200); //-20 dbm
	    		  LOG_INFO("RSSI: %d, Power: %d", rssi, -200);
	    	  }
	    	  else if(rssi <= -45 && rssi > -55)
	    	  {
	    		  gecko_cmd_system_set_tx_power(-150); //-15 dbm
	    		  LOG_INFO("RSSI: %d, Power: %d", rssi, -150);
	    	  }
	    	  else if(rssi <= -55 && rssi > -65)
	    	  {
	    		  gecko_cmd_system_set_tx_power(-50); //5 dbm
	    		  LOG_INFO("RSSI: %d, Power: %d", rssi, -50);
	    	  }
	    	  else if(rssi <= -65 && rssi > -75)
	    	  {
	    		  gecko_cmd_system_set_tx_power(0); //0 dbm
	    		  LOG_INFO("RSSI: %d, Power: %d", rssi, 0);
	    	  }
	    	  else if(rssi <= -75 && rssi > -85)
	    	  {
	    		  gecko_cmd_system_set_tx_power(50); //5 dbm
	    		  LOG_INFO("RSSI: %d, Power: %d", rssi, 50);
	    	  }
	    	  else if(rssi <= -85)
	    	  {
	    		  gecko_cmd_system_set_tx_power(TX_POWER_MAX); //10 dbm
	    		  LOG_INFO("RSSI: %d, Power: %d", rssi, TX_POWER_MAX);
	    	  }
	    	  BTSTACK_CHECK_RESPONSE(gecko_cmd_system_halt(0));
	    	  break;

	      case gecko_evt_le_connection_opened_id:
	    	//A connection is just opened(a client has connected to the server)
	    	connection = evt->data.evt_le_connection_opened.connection;
	    	BTSTACK_CHECK_RESPONSE(gecko_cmd_le_connection_set_parameters(connection, 60, 60, 4, 80));
	        connection_Open_Flag = 1;
	        break;

	      case gecko_evt_le_connection_closed_id:
	    	BTSTACK_CHECK_RESPONSE(gecko_cmd_system_halt(1));
	    	gecko_cmd_system_set_tx_power(0);
	    	BTSTACK_CHECK_RESPONSE(gecko_cmd_system_halt(0));
	    	connection_Open_Flag = 0;
	    	indicate_Temp_Flag = 0;
	        /* Check if need to boot to dfu mode */
	        if (boot_to_dfu) {
	          /* Enter to DFU OTA mode */
	        	//gecko_cmd_system_reset(2);
	        } else {
	          /* Stop timer in case client disconnected before indications were turned off */
	          //gecko_cmd_hardware_set_soft_timer(0, 0, 0);
	          /* Restart advertising after client has disconnected */
	        	BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_start_advertising(0, le_gap_general_discoverable, le_gap_connectable_scannable));
	        }
	        break;

	      case gecko_evt_system_external_signal_id:
	    	  //check rssi every 3 seconds
	    	  if(evt->data.evt_system_external_signal.extsignals & LETIMER0_UF_EVENT)
	    	  {
	    		  //gecko_cmd_le_connection_get_rssi(connection);
	    	  }
	    	  break;

	      case gecko_evt_hardware_soft_timer_id:
	    	  break;


	      default:
	        break;
	    }
}

//this code is based on the temperature measure algorithm in the soc_thermometer example from Simplicity Studio
void update_temperature(float temp)
{
	uint8_t htmTempBuffer[5]; /* Stores the temperature data in the Health Thermometer (HTM) format. */
	uint8_t flags = 0x00;   /* HTM flags set as 0 for Celsius, no time stamp and no temperature type. */
	uint32_t temperature;   /* Stores the temperature data read from the sensor in the correct format */
	uint8_t *p = htmTempBuffer; /* Pointer to HTM temperature buffer needed for converting values to bitstream. */

 /* Convert flags to bitstream and append them in the HTM temperature data buffer (htmTempBuffer) */
	UINT8_TO_BITSTREAM(p, flags);

	/* Convert sensor data to correct temperature format */
	temperature = FLT_TO_UINT32(temp, 0);
	/* Convert temperature to bitstream and place it in the HTM temperature data buffer (htmTempBuffer) */
	UINT32_TO_BITSTREAM(p, temperature);

	/* Send indication of the temperature in htmTempBuffer to all "listening" clients.
	 * This enables the Health Thermometer in the Blue Gecko app to display the temperature.
	 *  0xFF as connection ID will send indications to all connections. */
	/*if(indicate_Temp_Flag == 1)
	{
		BTSTACK_CHECK_RESPONSE(gecko_cmd_gatt_server_send_characteristic_notification(
				 0xFF, gattdb_temperature_measurement, 5, htmTempBuffer));
	}*/
}
