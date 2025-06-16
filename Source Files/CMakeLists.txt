cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_COMPILER /usr/bin/g++)
include(pico_sdk_import.cmake)

project(rcV4)

pico_sdk_init()

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include_directories(/home/Downloads/component_libs/lib)
include_directories(${CMAKE_SOURCE_DIR}/lib)
add_subdirectory(lib/nrf24l01)

add_executable(rcV4
 	rcv4.c
 	lcd_driver.pio
 	cdhc_in.pio
 	)
 	
pico_generate_pio_header(rcV4 ${CMAKE_CURRENT_LIST_DIR}/lcd_driver.pio)
pico_generate_pio_header(rcV4 ${CMAKE_CURRENT_LIST_DIR}/cdhc_in.pio)

target_include_directories(rcV4 PUBLIC 
	/home/alphonse/Downloads/component_libs
	/home/alphonse/Downloads/component_libs/displays
	/home/alphonse/Downloads/component_libs/FatFs-main
	/home/alphonse/Downloads/component_libs/hi_lvl_resources
	/home/alphonse/Downloads/component_libs/Periphereals
	/home/alphonse/Downloads/component_libs/Screen_lib
	)

target_sources(rcV4 PRIVATE rcv4.c)

pico_enable_stdio_usb(rcV4 1)
pico_enable_stdio_uart(rcV4 0)
pico_add_extra_outputs (rcV4)


target_link_libraries(rcV4
      pico_stdlib
      pico_time
      nrf24_driver
      hardware_gpio
      hardware_pio
      hardware_spi
      hardware_adc
      hardware_i2c
      hardware_dma
      hardware_pwm
      
)

