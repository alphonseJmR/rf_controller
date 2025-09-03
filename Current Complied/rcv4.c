#include <stdio.h>
#include "stdlib.h"
#include "Poly_Tools.h"
#include "../pio_files/resource/controller_configurations.h"
#include "cdhc_in.pio.h"

char spare_entry[20];
char spare1_entry[20];

// This is our define selector
#define client_transmitter

  // Our NRF client
nrf_client_t trans_client;


// Display encoder values, pwm info
ssd_i2c tri_encoder_screen = {
  .inst = i2c1,
  .scl = 19,
  .sda = 18,
  .b_rate = 50000,
  .dev_id = 0x3C
};


// display - channel, sig strength, rate
ssd_i2c nrf_screen = {
  .inst = i2c0,
  .scl = 21,
  .sda = 20,
  .b_rate = 50000,
  .dev_id = 0x3C
};


//  display - we'll figure it out
ssd_i2c yet_to_decide_screen = {
  .inst = i2c0,
  .b_rate = 50000,
  .dev_id = 0x3D
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
      sprintf(line1_entry, "Channel: %3i ", IO->re1_v);
  //    write_general_string(&nrf_screen, s2_temp_buf, 0, 0, line1_entry, false);
      render_general_string(&nrf_screen, s2_temp_buf, 0, 0, &frame, line1_entry, 2);
  //    render(&nrf_screen, s2_temp_buf, &frame, 2);
    }else{
      //  channel 110 will be our default.
      sprintf(line1_entry, "Default: %3i  ", 110);
  //    write_general_string(&nrf_screen, s2_temp_buf, 0, 0, line1_entry, false);
      render_general_string(&nrf_screen, s2_temp_buf, 0, 0, &frame, line1_entry, 2);
  //    render(&nrf_screen, s2_temp_buf, &frame, 2);
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
      //    write_general_string(&nrf_screen, s2_temp_buf, 10, 0, , false);
          render_general_string(&nrf_screen, s2_temp_buf, 10, 0, &frame, "Power:   -18dBm    ", 2);
      //    render(&nrf_screen, s2_temp_buf, &frame, 2);
        break;

        case -12:
      //    write_general_string(&nrf_screen, s2_temp_buf, 10, 0, "Power:   -12dBm    ", false);
          render_general_string(&nrf_screen, s2_temp_buf, 10, 0, &frame, "Power:   -12dBm    ", 2);
      //    render(&nrf_screen, s2_temp_buf, &frame, 2);
          break;
        
        case -6:
      //    write_general_string(&nrf_screen, s2_temp_buf, 10, 0, "Power:   -6dBm     ", false);
          render_general_string(&nrf_screen, s2_temp_buf, 10, 0, &frame, "Power:   -6dBm    ", 2);
      //    render(&nrf_screen, s2_temp_buf, &frame, 2);
          break;

        case 0:
      //    write_general_string(&nrf_screen, s2_temp_buf, 10, 0, "Power:    0dBm     ", false);
          render_general_string(&nrf_screen, s2_temp_buf, 10, 0, &frame, "Power:   0dBm    ", 2);
      //    render(&nrf_screen, s2_temp_buf, &frame, 2);
          break;
        
        default:
      //    write_general_string(&nrf_screen, s2_temp_buf, 10, 0, "Pwr Lvl Entry Error", false);
          render_general_string(&nrf_screen, s2_temp_buf, 10, 0, &frame, "Pwr Lvl Entry Error", 2);
      //    render(&nrf_screen, s2_temp_buf, &frame, 2);
          IO->re2_v = -18;
          break;
      }
    
        config->power = IO->re2_v;
      
    }else{
      IO->re2_v = -18;
    //  write_general_string(&nrf_screen, s2_temp_buf, 10, 0, "Default:   -18dBm  ", false);
      render_general_string(&nrf_screen, s2_temp_buf, 10, 0, &frame, "Default:   -18dBm  ", 2);
    //  render(&nrf_screen, s2_temp_buf, &frame, 2);
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
    //  write_general_string(&nrf_screen, s2_temp_buf, 20, 0, "Data Rate:  1 MBPS  ", false);
      render_general_string(&nrf_screen, s2_temp_buf, 20, 0, &frame, "Data Rate:  1 MBPS  ", 2);
    //  render(&nrf_screen, s2_temp_buf, &frame, 2);
    }else if(IO->re3_v == 2){
    //  write_general_string(&nrf_screen, s2_temp_buf, 20, 0, "Data Rate:  2 MBPS  ", false);
      render_general_string(&nrf_screen, s2_temp_buf, 20, 0, &frame, "Data Rate:  2 MBPS  ", 2);
    //  render(&nrf_screen, s2_temp_buf, &frame, 2);
    }else if(IO->re3_v == 3){
    //  write_general_string(&nrf_screen, s2_temp_buf, 20, 0, "Data Rate: 250 KBPS ", false);
      render_general_string(&nrf_screen, s2_temp_buf, 20, 0, &frame, "Data Rate:  250 KBPS  ", 2);
    //  render(&nrf_screen, s2_temp_buf, &frame, 2);
    }else{
      //  There was an uncaaught error.
      printf("Uncaught error, default value is 250kbps.\n");
    //  write_general_string(&nrf_screen, s2_temp_buf, 20, 0, "Data Rate: 250 KBPS ", false);
      render_general_string(&nrf_screen, s2_temp_buf, 20, 0, &frame, "Data Rate:  250 KBPS  ", 2);
    //  render(&nrf_screen, s2_temp_buf, &frame, 2);
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


void dsp_trd_scrn(ssd_i2c *cfg){
  i2c_init(cfg->inst, cfg->b_rate);
  ssd_screen_init(cfg);
      draw_line(cfg, s3_temp_buf, 40, 32, 80, 32, true);
    write_general_string(cfg, s3_temp_buf, 24, 0, "SSD1306", true);
    write_general_string(cfg, s3_temp_buf, 24, 10, "Figure Out", true);
    write_general_string(cfg, s3_temp_buf, 24, 20, " Use For This  ", true);
    write_general_string(cfg, s3_temp_buf, 24, 30, "   Display   ", true);
    render(cfg, s3_temp_buf, &frame, 3);
    clear_shadow_buffer(3);
    i2c_deinit(cfg->inst);
}


void write_adc_screen(ssd_i2c *cfg, ADC *in){

  easy_map(&in->ADC_1);
  easy_map(&in->ADC_2);
    sprintf(spare_entry, "Vert: %3i.\n", in->ADC_1.mapped);
  write_general_string(cfg, s3_temp_buf, 0, 0, spare_entry, false);
    sprintf(spare1_entry, "Hori: %3i.\n", in->ADC_2.mapped);
  write_general_string(cfg, s3_temp_buf, 10, 0, spare1_entry, false);

  send_to_screen(cfg, s3_temp_buf, 3);
  
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

//  read_i2c_addr(i2c0);

//  read_i2c_addr(i2c1);

   setup_rf_controller_ssd1306(&tri_encoder_screen, 1);
   setup_rf_controller_ssd1306(&nrf_screen, 2);
   dsp_trd_scrn(&yet_to_decide_screen);
  // setup_rf_controller_ssd1306(&yet_to_decide_screen, 3, false);


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


  printf("\n\tInitalization Flags Check: 0x%02x.\n", timer_flags);

  if(timer_flags != 0x07){
    main_run_b = false;
    printf("Init Error.\n");

      sleep_ms(750);
  }else{
    main_run_b = true;
    printf("Init Successful.\n");

      sleep_ms(750);
  }
    write_adc_screen(&yet_to_decide_screen, &analog_sticks);
  setup_nrf_init_vals(&controller, &trans_client, &controller_rf);
  nrf_client_configured(&trans_client, &trans_pins, &controller, 2000000);
  
while(main_run_b) {
  if(a < 14){

  a++;
  }else{
    a = 0;
  }
//  clear_screen(&lcd);

printf("Rotary 1: %i\n", controller_rf.re1_v);
printf("Rotary 2: %i\n", controller_rf.re2_v);
printf("Rotary 3: %i\n\n", controller_rf.re3_v);
sleep_ms(400);

}

tight_loop_contents();
}
