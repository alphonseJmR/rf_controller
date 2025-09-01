#include <stdio.h>
#include "stdlib.h"
#include "Poly_Tools.h"
#include "../pio_files/resource/controller_configurations.h"
#include "cdhc_in.pio.h"
#include "lcd_driver.pio.h"

// This is our define selector
#define client_transmitter

  // Our NRF client
nrf_client_t trans_client;

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

nrf_manager_t controller = {

    // AW_3_BYTES, AW_4_BYTES, AW_5_BYTES
    .address_width = AW_5_BYTES,

    // dynamic payloads: DYNPD_ENABLE, DYNPD_DISABLE
    .dyn_payloads = DYNPD_ENABLE,

    // data rate: RF_DR_250KBPS, RF_DR_1MBPS, RF_DR_2MBPS
    .data_rate = RF_DR_1MBPS,

    // RF_PWR_NEG_18DBM, RF_PWR_NEG_12DBM, RF_PWR_NEG_6DBM, RF_PWR_0DBM
    .power = RF_PWR_NEG_18DBM,

    // retransmission count: ARC_NONE...ARC_15RT
    .retr_count = ARC_10RT,

    // retransmission delay: ARD_250US, ARD_500US, ARD_750US, ARD_1000US
    .retr_delay = ARD_500US 
  
};


repeating_timer_t calc_val;
repeating_timer_t dma_chan;
repeating_timer_t adc_runn;

// This function will take user input to setup the necessary nrf configuration.
// this function currently requires the use of the reir struct
// could be implemented with the numpad as well.
void setup_nrf_init_vals(nrf_manager_t *config, nrf_client_t *client, reir *IO){

  // create save point for user input deltas, so we can use our own system.
  int16_t pre_a, pre_b, pre_c = 0x0000;
  pre_a = IO->delta1;
  pre_b = IO->delta2;
  pre_c = IO->delta3;

  IO->delta1 = 1;
  IO->delta2 = 6;
  IO->delta3 = 1;

  IO->re1_v = 110;
  IO->re2_v = -18;
  IO->re3_v = 1;

  char line1_entry[20];
  char line2_entry[20];

  bool bool_buffer = false;
    //  Our default value should be set as 110

  while(!bool_buffer){
    //  printf("bool1 status: %i.\n", (uint8_t)IO->switch1);
    //  printf("bool2 status: %i.\n", (uint8_t)IO->switch2);
    printf("Current Channel: %i.\n", IO->re1_v);
    if(IO->re1_v < 121 && IO->re1_v >= 110){
      config->channel = IO->re1_v;
      sprintf(line1_entry, "Enter Channel: %3i ", IO->re1_v);
      _2004_send_string_at_line(&lcd, line1_entry, 1, 0);
    }else{
      //  channel 110 will be our default.
      sprintf(line1_entry, "Safe Channel: %3i  ", 110);\
      _2004_send_string_at_line(&lcd, line1_entry, 1, 0);
      config->channel = 110;
      IO->re1_v = 110;
    }

    bool_buffer = IO->switch1;
  }

  //  printf("Safe to Release\n");
      sleep_us(200);
    bool_buffer = false;
    IO->switch1 = false;
    IO->re1_v = 0;
    for(uint8_t a = 0; a < 20; a++){
      line1_entry[a] = ' ';
    }

  while(!bool_buffer){

    if(IO->re2_v > -18 || IO->re2_v < 1){
      switch(IO->re2_v){
        case -18:
          sprintf(power, "Power:   -18dBm    ");
          _2004_send_string_at_line(&lcd, power, 2, 0);
        break;

        case -12:
          sprintf(power, "Power:   -12dBm    ");
          _2004_send_string_at_line(&lcd, power, 2, 0);
          break;
        
        case -6:
          sprintf(power, "Power:   -6dBm     ");
          _2004_send_string_at_line(&lcd, power, 2, 0);
          break;

        case 0:
          sprintf(power, "Power:    0dBm     ");
          _2004_send_string_at_line(&lcd, power, 2, 0);
          break;
        
        default:
          sprintf(power, "Pwr Lvl Entry Error");
          _2004_send_string_at_line(&lcd, power, 2, 0);
          IO->re2_v = -18;
          break;
      }
    
        config->power = IO->re2_v;
      
    }else{
      IO->re2_v = 0;
      sprintf(power, "Default:   -18dBm  ");
      _2004_send_string_at_line(&lcd, power, 2, 0);
      config->power = RF_PWR_NEG_18DBM;
    }

      bool_buffer = IO->switch1;

  }
  //  printf("Safe to Release\n");
      sleep_us(200);
    bool_buffer = false;
    IO->switch1 = false;
    IO->re2_v = 0;
    for(uint8_t a = 0; a < 20; a++){
      power[a] = ' ';
    }

  while(!bool_buffer){
    
    if(IO->re3_v == 1){
      sprintf(data_rate, "Data Rate:  1 MBPS  ");
      _2004_send_string_at_line(&lcd, data_rate, 3, 0);
    }else if(IO->re3_v == 2){
      sprintf(data_rate, "Data Rate:  2 MBPS  ");
      _2004_send_string_at_line(&lcd, data_rate, 3, 0);
    }else if(IO->re3_v == 3){
      sprintf(data_rate, "Data Rate: 250 KBPS ");
      _2004_send_string_at_line(&lcd, data_rate, 3, 0);
    }else{
      //  There was an uncaaught error.
      sprintf(data_rate, "Default:  250 KBPS  ");
      _2004_send_string_at_line(&lcd, data_rate, 3, 0);
    }

    bool_buffer = IO->switch1;
  }
  switch(IO->re3_v){
    case 1:
      config->data_rate = RF_DR_1MBPS;
      break;
    case 2:
      config->data_rate = RF_DR_2MBPS;
      break;
    case 3:
      config->data_rate = RF_DR_250KBPS;
      break;
    default:
      config->data_rate = RF_DR_250KBPS;
      break;
  }
  //  printf("Safe to Release\n");
      sleep_us(200);
    bool_buffer = false;
    IO->switch1 = false;
    IO->re3_v = 0;
    for(uint8_t a = 0; a < 20; a++){
      data_rate[a] = ' ';
    }

//  can't forget to reset the initial values from the user!
    IO->delta1 = pre_a;
    IO->delta2 = pre_b;
    IO->delta3 = pre_c;

}

// loaded with "generic_setup" reir struct
bool calc_controller_rotary_pos(repeating_timer_t *rt){
  rotary_gauge(&controller_rf);
  return true;
}


int main() {
  uint16_t buffer = 0;
  uint8_t a = 0;
  // Toggler if main program can run.
  // create a struct for ALL bools, or at least buffer them
  bool main_run_b = false;
  uint8_t timer_flags = 0x00;

  stdio_init_all();
//  remove the 7k and 4k delays in prod.  those are just to connect to the terminal in time.
  sleep_ms(7000);
  adc_init();
  sleep_ms(4000);
  printf("Main Program Setting Up Now.\n");

  init_rotary_encoder_program(&controller_rf);
    if(!setup_rep_dma(&controller_dma, &controller_rf, &dma_c2, 500)){
      printf("CDHC165 PIO Error.\tCode: 0x01\n");
      timer_flags |= 0x00;
    }else{
      printf("CDHC165 PIO Started.\n");

      if(add_repeating_timer_us(200, calc_controller_rotary_pos, NULL, &reir_timer)){
        timer_flags |= 0x02;
      }

      timer_flags |= 0x01;
    }

  ADC_X_Initialisation(&analog_sticks);
    if(!add_repeating_timer_us(150, read_adc, NULL, &adc_runn)){
      printf("ADC Timer Error.\tCode: 0x04\n");
        timer_flags |= 0x00;
    }else{
      printf("ADC Timer Started\n");
      timer_flags |= 0x04;
    }

    init_lcd_driver_program(&lcd);
    init_2004_lcd(&lcd);

    send_home(&lcd);
    send_char(&lcd, 'T');
    sleep_ms(1000);
  _2004_send_string_at_line(&lcd, "   RF Controller   ", 0, 0);
  _2004_send_string_at_line(&lcd, "    Version 4.1    ", 1, 0);
  _2004_send_string_at_line(&lcd, "     Authored By   ", 2, 0);
  _2004_send_string_at_line(&lcd, "      Alphonse     ", 3, 0);
  sleep_ms(1000);

  printf("\n\tInitalization Flags Check: 0x%02x.\n", timer_flags);

  if(timer_flags != 0x05 || timer_flags != 0x07){
    main_run_b = false;
    printf("Init Error.\n");
      clear_screen(&lcd);
      _2004_send_string_at_line(&lcd, init_err, 1, 0);
      _2004_send_string_at_line(&lcd, init_err1, 2, 0);
      sleep_ms(750);
  }else{
    main_run_b = true;
    printf("Init Successful.\n");
      clear_screen(&lcd);
      _2004_send_string_at_line(&lcd, init_tex, 1, 0);
      _2004_send_string_at_line(&lcd, init_text, 2, 0);
      sleep_ms(750);
  }

  setup_nrf_init_vals(&controller, &trans_client, &controller_rf);
  nrf_client_configured(&trans_client, &trans_pins, &controller, 2000000);

while(main_run_b) {
//  clear_screen(&lcd);

printf("Rotary 1: %i\n", controller_rf.re1_v);
printf("Rotary 2: %i\n", controller_rf.re2_v);
printf("Rotary 3: %i\n\n", controller_rf.re3_v);
sleep_ms(400);

}

tight_loop_contents();
}
