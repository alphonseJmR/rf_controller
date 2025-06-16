#ifndef RADIO_CONTROLLER_VERSION_4_CONFIGURATIONS_S_H
#define RADIO_CONTROLLER_VERSION_4_CONFIGURATIONS_S_H

#include "controller_structures.h"

pio_cdhc cdhc = {
  .Pins = {
    .lat = 1,
    .clk = 3,
    .inh = 2,
    .data = 0
  },
  .Pio = {
    .port = pio0,
    .sm = 0,
    .can_add = false
  }
};

lcd_pio lcd = {
  .port = pio1,
  .sm = 0,
  .rs = 20,
  .en = 21,
  .d4 = 19,
  .d5 = 18,
  .d6 = 17,
  .d7 = 16
};

pio_rot rotary_data = {
  .rot_a_inc = 1,
  .rot_b_inc = 2,
  .rot_c_inc = 30
};

demas reg_in = {
  .r_inc = false,
  .w_inc = false
};

ADC joy_stick = {

  .ADC_1 = {
    .adc_instance = 0,
    .pinum = 26,
    .mapped = 0,
    .miin = 0,
    .main = 4095,
    .miout = 0,
    .maout = 2047,
    .raw_avg = 0
  },

  .ADC_2 = {
    .adc_instance = 1,
    .pinum = 27,
    .mapped = 0,
    .miin = 0,
    .main = 4095,
    .miout = 0,
    .maout = 2047,
    .raw_avg = 0
  },

  .ADC_3 = {

    .adc_instance = 2,
    .pinum = 28
  }

};

//  Our current controller will use 5 channels?
mcp_s adc_five = {
  .port = pio0,
  .sm = 1,
  .clk_sm = 2,                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          
  .d_out = 6,
  .clock = 9,
  .cs_n = 7,
  .d_in = 8

};

#endif