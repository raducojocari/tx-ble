#include <ArduinoBLE.h>
#include "EmonLib.h"

EnergyMonitor emon1;
 
BLEService energyMonitorService("691263ec-5983-4034-8eea-399c82e7fa7a");

BLEFloatCharacteristic energyUsageChar("54fd1583-b8ca-4153-a72f-92d846b041bc",
    BLERead | BLENotify);

float CALIBRATION = 43;
long previousMillis = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  pinMode(LED_BUILTIN, OUTPUT);

  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    while (1);
  }

  initialeBleService();
  Serial.println("Bluetooth device active, waiting for connections...");

  emon1.current(1, CALIBRATION);
}

void loop() {
  // wait for a BLE central
  BLEDevice central = BLE.central();

  // if a central is connected to the peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's BT address:
    Serial.println(central.address());
    // turn on the LED to indicate the connection:
    digitalWrite(LED_BUILTIN, HIGH);

    while (central.connected()) {

      readEnergyUsage();
      digitalWrite(LED_BUILTIN, HIGH);
       long currentMillis = millis();
      // if 200ms have passed
      if (currentMillis - previousMillis >= 200) {
        previousMillis = currentMillis;
         digitalWrite(LED_BUILTIN, LOW);
      }
    }
   // when the central disconnects, turn off the LED:
    digitalWrite(LED_BUILTIN, LOW);
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}

void readEnergyUsage() {
  float value = emon1.calcIrms(1480);
  
  Serial.println(value*222);

  energyUsageChar.writeValue(value*222);
}

void initialeBleService(){
  BLE.setLocalName("Energy Monitor");
  BLE.setAdvertisedService(energyMonitorService);
  energyMonitorService.addCharacteristic(energyUsageChar); 
  BLE.addService(energyMonitorService); 
  energyUsageChar.writeValue(0);
  BLE.advertise();
}
