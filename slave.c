/**
 * \file
 *         Lab1 Slave
 * \author
 *         Yannik Sembritzki <yannik@sembritzki.org>
 *
 */

#include "contiki.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include "dev/leds.h"
#include "command.h"

#include <string.h>
#include <stdio.h> /* For printf() */
#include <random.h>

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "Slave"
#define LOG_LEVEL LOG_LEVEL_INFO

#include "helpers.h"

/* Configuration */
#define SEND_INTERVAL (1 * CLOCK_SECOND)

/*---------------------------------------------------------------------------*/
PROCESS(slave_process, "Slave");
AUTOSTART_PROCESSES(&slave_process);

/*---------------------------------------------------------------------------*/
void input_callback(const void *data, uint16_t len,
  const linkaddr_t *src, const linkaddr_t *dest)
{
  if(len == sizeof(struct command)) {
    static int seq_id_received = 0;
    struct command cmd;
    memcpy(&cmd, data, sizeof(cmd));
    if (cmd.command == COMMAND_TOGGLE_LED) {
      if (cmd.seq_id <= seq_id_received) return;
      if (cmd.hops > 10) return; 
      seq_id_received = cmd.seq_id;

      LOG_INFO("Received LED setting %u\n", cmd.data);
      leds_set(1 << (4 + cmd.data));

      cmd.hops += 1;
      nullnet_buf = (uint8_t *)&cmd;
      nullnet_len = sizeof(cmd);
      NETSTACK_NETWORK.output(NULL);

    } else {
      log_unknown_command(cmd, src);
    }
  }
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(slave_process, ev, data)
{
  PROCESS_BEGIN();

  static struct etimer periodic_timer;
  nullnet_set_input_callback(input_callback);

  etimer_set(&periodic_timer, SEND_INTERVAL);
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

    etimer_reset(&periodic_timer);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
