#ifndef COMMAND_H
#define COMMAND_H
struct command {
    uint8_t seq_id;
    uint8_t hops;
    uint8_t command;
    uint8_t data;
};

#define COMMAND_TOGGLE_LED 	1

#endif
