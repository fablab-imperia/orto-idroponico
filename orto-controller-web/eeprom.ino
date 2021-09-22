// EEPROM memory map
/*
  /-------------------------------------------------------------------------\
  | address |                 parameter                 |  unit of measure  |
  |-------------------------------------------------------------------------|
  | 0       | checksum*                                 |      -            |
  | 1       | threshold_value_ph*100                    |      pH           |
  | 2       | threshold_value_conductivity   LOW BYTE   |      uS/cm        |
  | 3       | threshold_value_conductivity   HIGH BYTE  |      uS/cm        |
  | 4       | time_peristaltic_pump_on  LOW BYTE        |      ms           |
  | 5       | time_peristaltic_pump_on  MIDDLE BYTE     |      ms           |
  | 6       | time_peristaltic_pump_on  HIGH BYTE       |      ms           |
  | 7       | time_between_sensor_reads  LOW BYTE       |      s            |
  | 8       | time_between_sensor_reads  HIGH BYTE      |      s            |
  | 9       | time_water_pump_cycle  LOW BYTE           |      s            |
  | 10      | time_water_pump_cycle  MIDDLE BYTE        |      s            |
  | 11      | time_water_pump_cycle  HIGH BYTE          |      s            |
  | 12      | time_water_pump_active  LOW BYTE          |      s            |
  | 13      | time_water_pump_active  MIDDLE BYTE       |      s            |
  | 14      | time_water_pump_active  HIGH BYTE         |      s            |
  \-------------------------------------------------------------------------/

  the checksum byte is calculated as the XOR of bytes from 1 to 14 (included)
  so the XOR of bytes from 1 to 14 (included) should be 0 for valid data

  NOTE: in case of invalid checksum or if all 14 bytes are 0 or 255 will be used the default parameters

*/


void save_configuration (char parameter) {
  switch (parameter) {
    case '0':
      EEPROM.write(1, (uint8_t)(threshold_value_ph * 10));
      break;
    case '1':
      EEPROM.write(2, (threshold_value_conductivity & 0b0000000011111111));
      EEPROM.write(3, (threshold_value_conductivity & 0b1111111100000000) >> 8);
      break;
    case '2':
      EEPROM.write(4, (time_peristaltic_pump_on & 0b000000000000000011111111));
      EEPROM.write(5, (time_peristaltic_pump_on & 0b000000001111111100000000) >> 8);
      EEPROM.write(6, (time_peristaltic_pump_on & 0b111111110000000000000000) >> 16);
      break;
    case '3':
      EEPROM.write(7, (time_between_sensor_reads & 0b0000000011111111));
      EEPROM.write(8, (time_between_sensor_reads & 0b1111111100000000) >> 8);
      break;
    case '4':
      EEPROM.write(9, (time_water_pump_cycle & 0b000000000000000011111111));
      EEPROM.write(10, (time_water_pump_cycle & 0b000000001111111100000000) >> 8);
      EEPROM.write(11, (time_water_pump_cycle & 0b111111110000000000000000) >> 16);
      break;
    case '5':
      EEPROM.write(12, (time_water_pump_active & 0b000000000000000011111111));
      EEPROM.write(13, (time_water_pump_active & 0b000000001111111100000000) >> 8);
      EEPROM.write(14, (time_water_pump_active & 0b111111110000000000000000) >> 16);
      break;
  }

  EEPROM.commit();
  delay(10);                            // Delay just to be safe

  if (parameter == '5') {
    EEPROM.write(0, calculate_checksum());
    EEPROM.commit();
  }
}



bool retrive_configuration () {
  bool is_0 = true;
  bool is_1 = true;

  uint8_t checksum = 0;
  for (uint8_t i = 0; i < EEPROM_USED_SIZE; i++) {
    uint8_t val = EEPROM.read(i);
//    Serial.println(val, BIN);
    checksum ^= val;

    if (i != 0) {
      if (val != 0b00000000) {
        is_0 = false;
      }
      if (val != 0b11111111) {
        is_1 = false;
      }
    }
  }

  if (is_0 || is_1 || checksum != 0) {
    Serial.println("Invalid parameters or checksum, using default values");
    return false;
  }

  threshold_value_ph = EEPROM.read(1)/10.0;
  threshold_value_conductivity = EEPROM.read(2) + (EEPROM.read(3) << 8);
  time_peristaltic_pump_on = EEPROM.read(4) + (EEPROM.read(5) << 8) + (EEPROM.read(6) << 16);
  time_between_sensor_reads = EEPROM.read(7) + (EEPROM.read(8) << 8);
  time_water_pump_cycle = EEPROM.read(9) + (EEPROM.read(10) << 8) + (EEPROM.read(11) << 16);
  time_water_pump_active = EEPROM.read(12) + (EEPROM.read(13) << 8) + (EEPROM.read(14) << 16);

  return true;
}



uint8_t calculate_checksum () {
  uint8_t checksum = 0;
  for (uint8_t i = 1; i < EEPROM_USED_SIZE; i++) {
    checksum ^= EEPROM.read(i);
  }

  return checksum;
}
