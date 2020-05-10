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

void sendData()
{
  NETSTACK_NETWORK.output(NULL);
}

void input_callback(const void *data, uint16_t len,
                    const linkaddr_t *src, const linkaddr_t *dest)
{
  if (len == sizeof(struct command))
  {
    static uint8_t seq_id_received = 0;
    static struct command cmd;
    memcpy(&cmd, data, sizeof(cmd));
    if (cmd.command == COMMAND_TOGGLE_LED)
    {
      if (cmd.seq_id <= seq_id_received && seq_id_received - cmd.seq_id < 128)
        return;

      seq_id_received = cmd.seq_id;

      LOG_INFO("Received LED setting %u, seq id: %u, hops: %u\n", cmd.data, cmd.seq_id, cmd.hops);
      leds_toggle(1 << (4 + cmd.data));

      nullnet_buf = (uint8_t *)&cmd;
      nullnet_len = sizeof(cmd);
      static struct ctimer timer;
      ctimer_set(&timer, CLOCK_SECOND * (random_rand() % 1000) / 1000, &sendData, NULL);

      //NETSTACK_NETWORK.output(NULL);
    }
    else
    {
      log_unknown_command(cmd, src);
    }
  }
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(slave_process, ev, data)
{
  PROCESS_BEGIN();

  nullnet_set_input_callback(input_callback);
  while (1)
  {
    PROCESS_YIELD();
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
