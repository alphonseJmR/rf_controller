#include "nrf24l01.h"
#include "../Poly_Tools.h"

// transmitter can be SPI0 and receiver can be SPI1
#define TX_START 0x9A
#define TX_END 0X9B
#define CONNECTION_SUCCESS 0x5B
#define DATA_SELECTION 0xC4

#define slice_8_1(a) (uint8_t)(a & 0xFF)
#define slice_8_2(a) (uint8_t)((a >> 8) & 0xFF)
#define slice_8_3(a) (uint8_t)((a >> 16) & 0xFF)
#define slice_8_4(a) (uint8_t)((a >> 24) & 0xFF)

#define transmitter


#define MOSI_PIN 15
#define MISO_PIN 12
#define SCK_PIN  14
#define CSN_PIN  13
#define CE_PIN   11

#ifdef transmitter
spi_inst_t *rf_spi = spi1;
#endif


/*start of low level functions, specific to the mcu and compiler*/

/*delay in miliseconds*/
void delay_function(uint32_t duration_ms)
{
    sleep_ms(duration_ms);
}

/*contains all SPI configuations, such as pins and control registers*/
/*SPI control: master, interrupts disabled, clock polarity low when idle, clock phase falling edge, clock up tp 1 MHz*/
void SPI_Initializer()
{
    spi_init(rf_spi, 5000000);
    gpio_init(MOSI_PIN);
    gpio_init(MISO_PIN);
    gpio_init(SCK_PIN);
    gpio_set_function(MOSI_PIN, GPIO_FUNC_SPI);
    gpio_set_function(MISO_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SCK_PIN, GPIO_FUNC_SPI);
    

}

/*contains all CSN and CE pins gpio configurations, including setting them as gpio outputs and turning SPI off and CE '1'*/
void pinout_Initializer()
{
        printf("Initializing CSN Pin: %i.\n", CSN_PIN);
    gpio_init(CSN_PIN);
    gpio_set_dir(CSN_PIN, GPIO_OUT);
        printf("Putting CSN pin HI.\n");
    gpio_put(CSN_PIN, 1);
    
        printf("Initializing CE Pin: %i.\n", CE_PIN);
    gpio_init(CE_PIN);
    gpio_set_dir(CE_PIN, GPIO_OUT);
        printf("Putting CE pin LOW.\n");
    gpio_put(CE_PIN, 0);
}

/*CSN pin manipulation to high or low (SPI on or off)*/
void nrf24_SPI(uint8_t input)
{
    gpio_put(CSN_PIN, input);

}


/*CE pin maniplation to high or low*/
void nrf24_CE(uint8_t input)
{
    gpio_put(CE_PIN, input);
    sleep_us(50);
}

/*1 byte SPI shift register send and receive routine*/
uint8_t SPI_send_command(uint8_t command)
{
    uint8_t returned_data = 0x00;

    spi_init(rf_spi, 5000000);
        spi_write_read_blocking(rf_spi, &command, &returned_data, 1);
    spi_deinit(rf_spi); 

    return returned_data;
}

