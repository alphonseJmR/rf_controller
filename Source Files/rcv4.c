#include <stdio.h>
#include "stdlib.h"
#include "Poly_Tools.h"
#include "cdhc_in.pio.h"
#include "lcd_driver.pio.h"

#define client_transmitter
uint8_t a;

  // Our NRF client
nrf_client_t trans_client;

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

void set_nrf_init_vals(nrf_manager_t *config, nrf_client_t *client, pio_rot *IO){
bool bool_buffer = false;
IO->r1 = 110;

  while(!bool_buffer){
    printf("bool1 status: %i.\n", (uint8_t)IO->r1_b);
    printf("bool2 status: %i.\n", (uint8_t)IO->r2_b);
  if(IO->r1 < 121){
    config->channel = IO->r1;
    sprintf(channel, "Enter Channel: %3i ", IO->r1);
    dsp_2004_str_at_pos(&lcd, channel, 0, 1);
  }else{
    //  channel 110 will be our default.
    sprintf(channel, "Safe Channel: %3i  ", 110);
    dsp_2004_str_at_pos(&lcd, channel, 0, 1);
    config->channel = 110;
    IO->r1 = 110;
  }

  bool_buffer = IO->r1_b;
  }
    printf("Safe to Release\n");
      sleep_ms(200);
    bool_buffer = false;
    IO->r1_b = false;
    IO->r1 = 0;
    for(uint8_t a = 0; a < 20; a++){
      channel[a] = ' ';
    }


  while(!bool_buffer){

    if(IO->r2 <= 6){
      if(IO->r2 == 0){
        sprintf(power, "Power:   -18dBm     ");
        dsp_2004_str_at_pos(&lcd, power, 0, 2);
      }else if(IO->r2 == 2){
        sprintf(power, "Power:    -12dBm    ");
        dsp_2004_str_at_pos(&lcd, power, 0, 2);
      }else if(IO->r2 == 4){
        sprintf(power, "Power:    -6dBm     ");
        dsp_2004_str_at_pos(&lcd, power, 0, 2);
      }else if(IO->r2 == 6){
        sprintf(power, "Power:   0dBm    ");
        dsp_2004_str_at_pos(&lcd, power, 0, 2);
      }
        config->power = IO->r1;
      
    }else{
      IO->r2 = 0;
      sprintf(power, "Default:   -18dBm   ");
      dsp_2004_str_at_pos(&lcd, power, 0, 2);
      config->power = RF_PWR_NEG_18DBM;
    }
      bool_buffer = IO->r1_b;

  }
    printf("Safe to Release\n");
      sleep_ms(200);
    bool_buffer = false;
    IO->r1_b = false;
    IO->r2 = 0;
    for(uint8_t a = 0; a < 20; a++){
      power[a] = ' ';
    }

  while(!bool_buffer){

    if(IO->r2 == 0){
      sprintf(data_rate, "Data Rate:  1 MBPS  ");
      dsp_2004_str_at_pos(&lcd, data_rate, 0, 3);
      config->data_rate = RF_DR_1MBPS;
    }else if(IO->r2 == 8){
      sprintf(data_rate, "Data Rate:  2 MBPS  ");
      dsp_2004_str_at_pos(&lcd, data_rate, 0, 3);
      config->data_rate = RF_DR_2MBPS;
    }else if(IO->r2 == 32){
      sprintf(data_rate, "Data Rate: 250 KBPS ");
      dsp_2004_str_at_pos(&lcd, data_rate, 0, 3);
      config->data_rate = RF_DR_250KBPS;
    }else{
      //  There was an uncaaught error.
      sprintf(data_rate, "Default:  250 KBPS  ");
      dsp_2004_str_at_pos(&lcd, data_rate, 0, 3);
      config->data_rate = RF_DR_250KBPS;
    }

    bool_buffer = IO->r1_b;
  }
    printf("Safe to Release\n");
      sleep_ms(200);
    bool_buffer = false;
    IO->r1_b = false;
    IO->r2 = 0;
    for(uint8_t a = 0; a < 20; a++){
      data_rate[a] = ' ';
    }

}

bool adc_map_update(repeating_timer_t *rt){
  if(a < 32){
    adc_pin_call(&anal_sticks.ADC_1, a);
    adc_pin_call(&anal_sticks.ADC_2, a);
    a++;
  }else{
    a = 0;
  }

  return true;
}

int main() {
  uint16_t buffer = 0;
  uint8_t a = 0;
  // Toggler if main program can run.
  bool main_run_b = false;
  //  Test if input timer can be added.
  bool input_timer = false;
  bool dma_timer = false;
  bool adc_timer = false;
  uint8_t timer_flags = 0x00;

  stdio_init_all();
//  remove the 7k and 4k delays in prod.  those are just to connect to the terminal in time.
  sleep_ms(7000);
  adc_init();
  sleep_ms(4000);
  printf("Main Program Setting Up Now.\n");

  init_cdhc_program(&cdhc, &reg_in, &rotary_data);
    input_timer = add_repeating_timer_us(-10, rotation_values, NULL, &calc_val);
      if(!input_timer){
        printf("CDHC165 PIO Timer Error.\tCode: 0x01\n");
        timer_flags |= 0x00;
      }else{
        printf("CDHC165 PIO Timer Started.\n");
        timer_flags |= 0x01;
      }

    dma_timer = add_repeating_timer_us(500, dma_fire, NULL, &dma_chan);
    if(!dma_timer){
      printf("DMA Channel Timer Error.\tCode: 0x02\n");
        timer_flags |= 0x00;
    }else{
      printf("DMA Channel Timer Started\n");
      timer_flags |= 0x02;
    }

    ADC_X_Initialisation(&anal_sticks);
    adc_timer = add_repeating_timer_us(150, adc_map_update, NULL, &adc_runn);
      if(!adc_timer){
      printf("ADC Timer Error.\tCode: 0x04\n");
        timer_flags |= 0x00;
    }else{
      printf("ADC Timer Started\n");
      timer_flags |= 0x04;
    }

  dma_channel_start(reg_in.channel);
  start_2004_lcd_driver(&lcd);

  printf("\n\tInitalization Flags Check: 0x%02x.\n", timer_flags);

  if(timer_flags != 0x07){
    main_run_b = false;
      clear_screen(&lcd);
      dsp_2004_str_at_pos(&lcd, init_err, 0, 2);
      dsp_2004_str_at_pos(&lcd, init_err1, 0, 3);
  }else{
    main_run_b = true;
    clear_screen(&lcd);
    dsp_2004_str_at_pos(&lcd, init_tex, 0, 2);
    dsp_2004_str_at_pos(&lcd, init_text, 0, 3);
  }
    sleep_us(500);

  set_nrf_init_vals(&controller, &trans_client, &rotary_data);
  nrf_client_configured(&trans_client, &trans_pins, &controller, 2000000);

rotary_data.r1 = 0;

while(main_run_b) {
//  clear_screen(&lcd);
  dma_channel_wait_for_finish_blocking(reg_in.channel);

  anal_sticks.ADC_1.mapped = map_adc_16bit(&anal_sticks.ADC_1);
  anal_sticks.ADC_2.mapped = map_adc_16bit(&anal_sticks.ADC_2);

  dsp_2004_str_at_pos(&lcd, adc_test, 0, 1);
    sprintf(vertical, "Vertial Input: %4i ", anal_sticks.ADC_1.mapped);
  dsp_2004_str_at_pos(&lcd, vertical, 0, 2);
    sprintf(horizontal, "Horizontal In: %4i ", anal_sticks.ADC_2.mapped);
  dsp_2004_str_at_pos(&lcd, horizontal, 0, 3);
    sprintf(numtesk, "RE: %3i : %3i : %3i ", rotary_data.r1, rotary_data.r2, rotary_data.r3);
  dsp_2004_str_at_pos(&lcd, numtesk, 0, 4);

    printf("Value of fifo DMA frr?: 0x%04x.\n", rotary_data.frr);
    printf("RAW value of rxf pio fifo register 0 0x%04x\n", pio0->rxf[0]);

printf("Rotary 1: %i\n", rotary_data.r1);
printf("Rotary 2: %i\n", rotary_data.r2);
printf("Rotary 3: %i\n\n", rotary_data.r3);
sleep_ms(400);

}

tight_loop_contents();
}
