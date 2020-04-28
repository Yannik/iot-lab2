/**
 * \file
 *         Lab2 Master
 * \author
 *         Yannik Sembritzki <yannik@sembritzki.org>
 *
 */

#include "contiki.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include "dev/leds.h"

#include <string.h>
#include <stdio.h> /* For printf() */
#include <random.h>

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "Master"
#define LOG_LEVEL LOG_LEVEL_INFO

#include "helpers.h"

/* Configuration */
#define SEND_INTERVAL (1 * CLOCK_SECOND)
#define NUM_MASTERS 1
#define NUM_SLAVES 4


/*---------------------------------------------------------------------------*/
PROCESS(master_process, "Master");
AUTOSTART_PROCESSES(&master_process);


/*---------------------------------------------------------------------------*/
PROCESS_THREAD(master_process, ev, data)
{
  PROCESS_BEGIN();

  static struct etimer periodic_timer;
  static struct command cmd;
  cmd.command = COMMAND_TOGGLE_LED;
  cmd.hops = 1;
  cmd.seq_id = 1;

  nullnet_buf = (uint8_t *)&cmd;
  nullnet_len = sizeof(cmd);

  etimer_set(&periodic_timer, SEND_INTERVAL);
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

    cmd.data = random_rand() % 3;
    leds_set(1 << (4 + cmd.data));

    LOG_INFO("Broadcasting LED %u \n", cmd.data);
    NETSTACK_NETWORK.output(NULL);

    cmd.seq_id++;
    etimer_reset(&periodic_timer);
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
