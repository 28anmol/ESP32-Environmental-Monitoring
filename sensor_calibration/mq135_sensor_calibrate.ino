// Manual Calibration of MQ135 Air Quality/Gas Detector Sensor using Arduino UNO

#define Pin A0
#define RL 20.0  // Adjust to 10 or 20kΩ per your module - See datasheet

void setup() 
{
  Serial.begin(9600);
  delay(3000); // Wait for sensor warm-up

  int raw = analogRead(Pin);
  float voltage = (raw * 5.0) / 1023.0;
  Serial.print("Analog: "); 
  Serial.print(raw);
  Serial.print(", Voltage: "); 
  Serial.println(voltage);

  if (voltage == 0) 
  {
    Serial.println("Voltage zero, can't calculate Rs.");
    return;
  }

  float Rs = RL * (5.0 - voltage) / voltage;
  Serial.print("Rs: ");
  Serial.println(Rs);

  float Ro = Rs / 3.6; // Using clean air ratio 3.6 for MQ135
  Serial.print("Ro: ");
  Serial.println(Ro);
}

void loop() 
{
  // Nothing to be done in this loop
}

