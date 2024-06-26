#ifndef SASI_HPP

#define SASI_HPP

#include "Arduino.h"

// amount of registers
#define LENGTH_REG 256

// states
#define ERROR_STATE 255
#define WAIT_STATE 99
#define START_STATE 1
#define W_READ_ADDRESS_1 10
#define W_READ_ADDRESS_2 11
#define WAIT_VALUE_DELIMITER 12
#define READ_DATA_1 13
#define READ_DATA_2 14
#define READ_DATA_3 15
#define READ_DATA_4 16
#define R_READ_ADDRESS_1 20
#define R_READ_ADDRESS_2 21

// Error types
#define NO_ERROR 0
#define NO_HEX_PARAMETER 1
#define WRONG_FUNCTION 2
#define WRONG_DELIMITER 3
#define NOT_IMPLEMENTED_YET 4

// Function prototypes
uint8_t hex_to_int(uint8_t hex_char);
void int_to_hex(uint16_t input, char output[], uint8_t output_length);
void send_error(uint8_t error_code, char output_frame[]);
void send_read_response(char address[], char data[], char output_frame[]);

class SASI {
    public:
        SASI();
        void step();
        uint16_t get_register(uint8_t address);
        void set_register(uint8_t address, uint16_t data);
    private:
        uint8_t received_byte;
        uint8_t state;
        uint8_t address;
        uint16_t data_buffer;
        uint16_t data_register[LENGTH_REG];
        char data_output[4];
        char address_output[2];
        char error_response[3] = {'X', '0', '\n'};
        char read_response[9] = {'R', '0', '0', 'V', '0', '0', '0', '0', '\n'};
};

#endif


