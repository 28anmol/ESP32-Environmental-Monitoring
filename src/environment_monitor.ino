/*
Libraries to interact with the hardware
*/

// ESP32 WROOM 32D
#include <WiFi.h>

// DHT11 humidity sensor library
#include <DHT.h>

// SPI interfacing library
#include <SPI.h>

// OLED Display libraries
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Dallas - DS18B20 temperature sensor library
#include <OneWire.h>
#include <DallasTemperature.h>

// Time library
#include <time.h>


// Setting up constants for the OLED display
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_MOSI 23
#define OLED_CLK  18
#define OLED_DC 4
#define OLED_CS 5
#define OLED_RESET  16

// Constant for DHT11 temperature sensor
#define DHTTYPE DHT11


// Defining ESp32 Pin for temperature and humidity sensor
uint8_t DsTempPin = 15;
uint8_t DHTPIN = 13;


// WiFi Credentials - SSID and Password
const char* ssid = "Your_SSID";
const char* password = "Your_PASSWORD";


// WiFi Server
WiFiServer server(80);



DHT dht(DHTPIN, DHTTYPE);
OneWire onewire(DsTempPin);
DallasTemperature sensor(&onewire);


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RESET, OLED_CS);


// Variables to read all sensors in a set interval of 1 minute
unsigned long sensorInterval = 60000;
unsigned long lastSensorRead = 0;


// Variables for storing temperature and humidity
float hum = 0;
float temp = 0;


// Defining constants for fetching time from NTP server
const char* ntpServer = "pool.ntp.org";
const long getOffset_sec = 19800;
const int dayLightOffset_sec = 0;


char dayStr[16] = "";
char dateStr[40] = "";
char timeStr[32] = "";
struct tm timeinfo;


// Variables for PM2.5 sensor
int pmLedPin = 25;
int pmAnalogPin = 34;
double pmDustDensity = 0;
char pmLevel[80] = "";


 

// Function to read the PM2.5 sensor
void pm_sensor_read(double *pmDustDensity, char *pmLevel)
{

  /*
  Dust density levels:

  0 - 12um3 : good or no dust
  12.1 - 35.4um3 : moderate dust
  35.5 - 55.4um3 : unhealthy for sensitive groups
  55.5 - 150.4um3 : unhealthy
  150.5 - 250.4um3 : Very Unhealthy
  >250.5um3 : Hazardous levels
  */


  digitalWrite(pmLedPin, LOW);
  delayMicroseconds(280);

  int pmDustRaw = analogRead(pmAnalogPin);
  delayMicroseconds(40);
  digitalWrite(pmLedPin, HIGH);
  delayMicroseconds(9680);

  double pmVoltage = ((double)pmDustRaw * 3.3) / 4095.0;  // converting ADC back into corresponding voltage   
  *pmDustDensity = max(0.0, (double)((pmVoltage - 0.6) * 166.7));   // in ug/m3 (micrograms per cubic meter)

  Serial.println();
  Serial.print("Raw: "); 
  Serial.println(pmDustRaw);
  Serial.print("Voltage: "); 
  Serial.println(pmVoltage);
  Serial.print("PM2.5 Density: "); 
  Serial.println(*pmDustDensity);

  if(*pmDustDensity >= 0 && *pmDustDensity <= 12.0)
  {
    strcpy(pmLevel, "Clean Air");
  }
  else if(*pmDustDensity >= 12.1 && *pmDustDensity <= 35.4)
  {
    strcpy(pmLevel, "Moderate");
  }
  else if(*pmDustDensity >= 35.5 && *pmDustDensity <= 55.4)
  {
   strcpy(pmLevel, "Unhealthy(Sen Grp)");
  }
  else if(*pmDustDensity >= 55.5 && *pmDustDensity <= 150.4)
  {
    strcpy(pmLevel, "Unhealthy");
  }
  else if(*pmDustDensity >= 150.5 && *pmDustDensity <= 250.4)
  {
    strcpy(pmLevel, "V. Unhealthy");
  }
  else if(*pmDustDensity >= 250.5)
  {
    strcpy(pmLevel, "Hazardous");
  }
}




// Function to read the ambient temperature sensor
void temperature_read(float *temp)
{
  const int max_try = 5;

  for(int i = 0; i < max_try; i++)
  {
    sensor.requestTemperatures();
    delay(750);
    float t = sensor.getTempCByIndex(0);

    if(t != DEVICE_DISCONNECTED_C && t != 85.0 && t != 0.0)
    {
      *temp = t;
      return;
    }
  }
  delay(100);

  *temp = 0.0;
  Serial.println("Failed to read the temperature sensor");

}




// Function to read the ambient humidity sensor
void humidity_read(float *hum)
{

  float humid = dht.readHumidity();

  if(isnan(humid))
  {
    *hum = 0;
    Serial.println("Failed to capture humidity reading!");
  }
  else
  {
    *hum = humid;
  }

}



// Setup function - works only once
void setup()
{

  Serial.begin(115200);
  delay(1000);


  pinMode(DHTPIN, INPUT);   //pin 13
  pinMode(DsTempPin, INPUT);  // pin 15
  pinMode(25, OUTPUT);    // pmLedPin
  pinMode(34, INPUT);  //pmAnalogPin


  dht.begin();
  sensor.begin();

  delay(2000);

  display.begin(SSD1306_SWITCHCAPVCC);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);

  Serial.println("Connecting to Wifi .....");
  WiFi.begin(ssid, password);

  int retry = 0;

  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    retry++;

    if(retry > 60)
    {
      Serial.println("Failed to connect, restarting ESP32...");
      ESP.restart();
    }

  }

  Serial.println("\nConnected to WiFi!");
  Serial.print("Your local IP: ");
  Serial.println(WiFi.localIP());

  server.begin();

  configTime(getOffset_sec, dayLightOffset_sec, ntpServer);

}




// Loop function - works indefinitely until microcontroller is powered up
void loop()
{

  unsigned long now = millis();


  if(now - lastSensorRead >= sensorInterval)
  {
    
    lastSensorRead = now;

    if(getLocalTime(&timeinfo))
    {
      strftime(dayStr, sizeof(dayStr), "%A", &timeinfo);
      strftime(dateStr, sizeof(dateStr), "%B %d %Y", &timeinfo);
      strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);
    }
    else
    {
      strncpy(dayStr, "N/A", sizeof(dayStr));
      strncpy(dateStr, "Error", sizeof(dateStr));
      strncpy(timeStr, "--:--:--", sizeof(timeStr));
    }

    humidity_read(&hum);
    temperature_read(&temp);
    pm_sensor_read(&pmDustDensity, pmLevel);


    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println("Day: " + String(dayStr));
    display.println("Date: " + String(dateStr));
    display.println("Time: " + String(timeStr));
    display.println("Humidity: " + String(hum) + "%");
    display.println("Temperature: " + String(temp) + "C");
    display.println("PM 2.5: " + String(pmDustDensity) + "ug/m3");
    display.println("PM2.5(Lvl): "+ String(pmLevel));
    display.display();
    Serial.println("Day: " + String(dayStr));
    Serial.println("Date: " + String(dateStr));
    Serial.println("Time: " + String(timeStr));   
    Serial.println("Humidity: " + String(hum) + "%");
    Serial.println("Temperature: " + String(temp) + "C");
    Serial.println("PM 2.5: " + String(pmDustDensity) + "ug/m3");
    Serial.println("Dust Level: " + String(pmLevel));
  }

  WiFiClient client = server.available();

  if(client)
  {
    Serial.println("New Client Connected!");
    String request = client.readStringUntil('\r');
    Serial.println(request);
    client.flush();

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println();
    client.println("<!DOCTYPE html>");
    client.println("<html>");
    client.println("<head>");
    client.println("<meta name='viewport' content='width=device-width, initial-scale=1'>");
    client.println("<style>");
    client.println("body { font-family: Arial; background: #1e1e2f; color: #f1f1f1; text-align: left; padding: 20px; }");
    client.println("h1 { font-size: 2.5em; color: #00ffd5; }");
    client.println(".card { background: #2a2a40; border-radius: 10px; padding: 20px; margin: auto; width: 80%; box-shadow: 0 4px 8px rgba(0,0,0,0.3); }");
    client.println("p { font-size: 1.5em; margin: 10px 0; }");
    client.println("</style>");
    client.println("</head>");
    client.println("<body>");
    client.println("<h1>ESP32 Environment Monitor</h1>");
    client.println("<div class='card'>");
    client.println("<p><strong>Day:</strong> " + String(dayStr) + "</p>");
    client.println("<p><strong>Date:</strong> " + String(dateStr) + "</p>");
    client.println("<p><strong>Time:</strong> " + String(timeStr) + "</p>");
    client.println("<p><strong>Temperature:</strong> " + String(temp) + " &deg;C</p>");
    client.println("<p><strong>Humidity:</strong> " + String(hum) + " %</p>");
    client.println("<p><strong>PM 2.5:</strong> " + String(pmDustDensity) + "ug/m3" + "</p>");
    client.println("<p><strong>PM 2.5 Level:</strong> " + String(pmLevel) + "</p>");
    client.println("</div>");
    client.println("</body>");
    client.println("</html>");

    delay(1);
    client.stop();
    Serial.println("Client has been disconneted!");
  }
  delay(1000);
}


