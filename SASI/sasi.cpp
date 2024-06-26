#include "Arduino.h"
#include "sasi.hpp"

SASI::SASI() {
    state = START_STATE;
    data_buffer = 0;
    for(int i = 0; i < LENGTH_REG; i++) {
        data_register[i] = 0;
    }
}


void SASI::step() {
    if(Serial.available() > 0) {
        received_byte = Serial.read();
        if(received_byte == 0xD) {
            state = START_STATE;
        }
        switch(state) {
            case START_STATE: //begin of message
                //function byte
                if(received_byte == 'W' || received_byte == 'w') {
                    state = W_READ_ADDRESS_1;
                }
                else if(received_byte == 'R' || received_byte == 'r') {
                    state = R_READ_ADDRESS_1;
                }
                else if(received_byte == 0xD) {
                    // do nothing
                }
                else {
                    send_error(WRONG_FUNCTION, error_response);
                    state = ERROR_STATE;
                }
                break;
            case W_READ_ADDRESS_1: //read first address byte
                address = 0;
                if(isHexadecimalDigit((char) received_byte)) {
                    address = hex_to_int(received_byte);
                    state = W_READ_ADDRESS_2;
                }
                else {
                    send_error(NO_HEX_PARAMETER, error_response);
                    state = ERROR_STATE;
                }
                break;
            case W_READ_ADDRESS_2: //read second address byte
                if(isHexadecimalDigit((char) received_byte)) {
                    address = address * 0x10 + hex_to_int(received_byte);
                    state = WAIT_VALUE_DELIMITER;
                }
                else {
                    send_error(NO_HEX_PARAMETER, error_response);
                    state = ERROR_STATE;
                }
                break;
            case WAIT_VALUE_DELIMITER: //Value delimiter
                if(received_byte == 'V' || received_byte == 'v') {
                    state = READ_DATA_1;
                }
                else {
                    send_error(WRONG_DELIMITER, error_response);
                    state = ERROR_STATE;
                }
                break;
            case READ_DATA_1: //read first value byte
                if(isHexadecimalDigit((char) received_byte)) {
                    data_buffer = hex_to_int(received_byte);
                    state = READ_DATA_2;
                }
                else {
                    send_error(NO_HEX_PARAMETER, error_response);
                    state = ERROR_STATE;
                }
                break;
            case READ_DATA_2: //read second data byte
                if(isHexadecimalDigit((char) received_byte)) {
                    data_buffer = data_buffer * 0x10 + hex_to_int(received_byte);
                    state = READ_DATA_3;
                }
                else {
                    send_error(NO_HEX_PARAMETER, error_response);
                    state = ERROR_STATE;
                }
                break;
            case READ_DATA_3: //read third data byte
                if(isHexadecimalDigit((char) received_byte)) {
                    data_buffer = data_buffer * 0x10 + hex_to_int(received_byte);
                    state = READ_DATA_4;
                }
                else {
                    send_error(NO_HEX_PARAMETER, error_response);
                    state = ERROR_STATE;
                }
                break;
            case READ_DATA_4: //read forth data byte
                if(isHexadecimalDigit((char) received_byte)) {
                    data_buffer = data_buffer * 0x10 + hex_to_int(received_byte);
                    data_register[address] = data_buffer;
                    state = WAIT_STATE;
                    send_error(NO_ERROR, error_response);
                }
                else {
                    send_error(NO_HEX_PARAMETER, error_response);
                    state = ERROR_STATE;
                }
                break;
            case R_READ_ADDRESS_1:
                address = 0;
                if(isHexadecimalDigit((char) received_byte)) {
                    address = hex_to_int(received_byte);
                    state = R_READ_ADDRESS_2;
                }
                else {
                    send_error(NO_HEX_PARAMETER, error_response);
                    state = ERROR_STATE;
                }
                break;
            case R_READ_ADDRESS_2: //read second address byte
                if(isHexadecimalDigit((char) received_byte)) {
                    address = address * 0x10 + hex_to_int(received_byte);
                    state = WAIT_STATE;
                    int_to_hex(address, address_output, sizeof(address_output));
                    int_to_hex(data_register[address], data_output, sizeof(data_output));
                    send_read_response(address_output, data_output, read_response);
                    //Serial.print("R");
                    //Serial.print(address_output);
                    //Serial.print("V");
                    //Serial.print(data_output);
                    //Serial.print("\n");
                }
                else {
                    send_error(NO_HEX_PARAMETER, error_response);
                    state = ERROR_STATE;
                }
                break;
            case ERROR_STATE: //error state
                //wait until reset 
                break;
            
            case WAIT_STATE: //wait state
                //wait until reset 
                break;

            default:
            send_error(NOT_IMPLEMENTED_YET, error_response);
        }
    }
}


uint16_t SASI::get_register(uint8_t address) {
    return(data_register[(int) address]);
}

void SASI::set_register(uint8_t address, uint16_t data) {
      data_register[(int) address] = data;
      
}

uint8_t hex_to_int(uint8_t hex_char) {
  if(hex_char <= '9') {
    return(hex_char - '0');
  }
  else if(hex_char <= 'Z') {
    return(hex_char - 'A' + 0xA);
  }
  else {
    return(hex_char - 'a' + 0xA);
  }
}

void int_to_hex(uint16_t input, char output[], uint8_t output_length) {
    for(int i = 0; i < output_length; i++) {
        uint8_t mod_remainder = input % 0x10;
        if (mod_remainder <= 9) {
            output[output_length - 1 - i] = mod_remainder + '0';
        }
        else {
            output[output_length - 1 - i] = mod_remainder + 'A' - 0xA;
        }        
        input /= 0x10;
    }
}

void send_error(uint8_t error_code, char output_frame[]) {
    if (error_code <= 9) {
        error_code += '0';
    }
    else {
        error_code += 'A' - 0xA;
    }
    output_frame[1] = error_code;
    Serial.write(output_frame, 3);
}

void send_read_response(char address[], char data[], char output_frame[]) {
    output_frame[1] = address[0];
    output_frame[2] = address[1];
    output_frame[4] = data[0];
    output_frame[5] = data[1];
    output_frame[6] = data[2];
    output_frame[7] = data[3];
    Serial.write(output_frame, 9);

}