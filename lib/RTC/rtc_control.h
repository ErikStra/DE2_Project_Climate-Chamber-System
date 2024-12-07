
#ifndef __RTC_CONTROL_H__
#define __RTC_CONTROL_H__
  /**
   * @desc   Voltmeter
   *
   * @param  void
   *
   * @return void
   
   */
#include <avr/io.h>
#include <stdlib.h>

  int rtc_control (void);

  uint8_t compare_time_with_sun(uint8_t currentHour, uint8_t sunrise, uint8_t sunset);

  void rtc_set_time(uint8_t hours, uint8_t minutes, uint8_t seconds);

  void rtc_initialize(void);

  void rtc_read_time(void);

  void load_settings_from_eeprom(void);

  void save_settings_to_eeprom(void);

  uint8_t bcdToDec(uint8_t val);

  uint8_t decToBcd(uint8_t val);

  //void rtc_control_interrupt(void);

  void rtc_control_loop(void);
  
  void rtc_control_init(void);
#endif
