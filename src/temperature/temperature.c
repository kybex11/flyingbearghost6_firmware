#include "./main.c"
#include <Thermistor.h>

int extruderThermistorPin = A0;
int bedThermistorPin = A1;

Thermistor extruderThermistor(extruderThermistorPin);
Thermistor bedThermistor(bedThermistorPin);

float extruderTemperature;
float bedTemperature;

void setup() {
  Serial.begin(9600);
}

void loop() {
  extruderTemperature = extruderThermistor.readTemperature();
  bedTemperature = bedThermistor.readTemperature();

  if (extruderTemperature < desiredExtruderTemperature) {
    HOTEND_HEATING_TEMP = MENU_SELECTED_HOTEND_TEMP;
  } else {
    HOTEND_HEATING_TEMP = 0;
  }

  if (bedTemperature < desiredBedTemperature) {
    BED_HEATING_TEMP = MENU_SELECTED_BED_TEMP;
  } else {
    BED_HEATING_TEMP = 0;
  }

  Serial.print("Extruder Temp: ");
  Serial.print(extruderTemperature);
  Serial.print(" Bed Temp: ");
  Serial.println(bedTemperature);

  delay(1000);
}

void setHotendTemp(float temp) {
  if (temp >= MAX_HOTEND_TEMP)
    analogWrite(extruderThermistorPin, sqrt(temp * temp));
}

void setBedTemp(float temp) {
  if (temp >= MAX_TABLE_TEMP)
    analogWrite(bedThermistorPin, sqrt(temp * temp));
}