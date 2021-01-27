#include <OneWire.h>                                     // Library used for DS18B20 sensor
#include <DallasTemperature.h>                           // Library used for DS18B20 sensor          
#include <LiquidCrystal.h>                               // Libraby used for LCD
#include <Adafruit_Sensor.h>                             // Library used for DS18B20 sensor
#include <DHT.h>                                         // Library used for DHT22 sensor
#include <Wire.h>                                        // Standard Arduino library used for BMP180 sensor
#include <SFE_BMP180.h>                                  // Library used for BMP180 sensor

#define dht_pin A0                                       // Pin connected to DHT22 sensor
#define dht_type DHT22                                   // Define DHT sensor type 
#define altitude 13                                      // Altitude inserted by user
const int DS18B20_pin = A1;                              // Pin connected to DS18B20 sensor
const int button_pin = 9;                                // Pin connected to button
const int red_led_pin =  8;                              // Pin connected to red LED 
const int green_led_pin =  7;                            // Pin connected to green LED
int button_state = 0;                                    // Variable called button_state
int push = 0;                                            // Variable called push
int button = 0;                                          // Variable called button
bool C_or_F = true;                                      // Variable C_or_F
int back_light_pin = 10;                                 // Pin connected to kathode of LCD
int pwm_back_light;                                      // Variable called pwm_back_light   

DHT dht = DHT(dht_pin ,dht_type);                        // Creates object dht
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);                   // LCD conneted pins
OneWire oneWire(DS18B20_pin);                            // Connects DS18B20 sensor to oneWire library
DallasTemperature DS18B20(&oneWire);                     // Creates object DS18B20
SFE_BMP180 BMP180;                                       // Creates object BMP180

void setup()
{
  pinMode(red_led_pin, OUTPUT);                          // Red LED pin mode - output
  pinMode(green_led_pin, OUTPUT);                        // Green LED pin mode - output
  pinMode(button_pin, INPUT_PULLUP);                     // Button pin mode - input_pullup
  pinMode(back_light_pin,INPUT_PULLUP);                  // Kathode of LCD pin mode - input_pullup
  
  lcd.begin(16, 2);                                      // LCD cursor position
  lcd.print("TEMP CISN   WILG");

  Serial.begin(9600);                            // Serial
  DS18B20.begin();                               // Starts DS18B20 sensor
  dht.begin();                                   // Starts DHT22 sensor
  BMP180.begin();                                // Starts BMP180 sensor
}

void loop()
{
  double tempC;                                          // Variable called temperature in C
  double tempF;                                          // Variable called temperaute in F
  double humidity = dht.readHumidity(dht_pin);           // Variable called humidity
  char stat;                                             // Variable called stats
  double t;                                              // Variable called temperature
  double pressure;                                       // Variable called absolute pressure
  double pressure0;                                      // Variable called sea-level compensated pressure
  double user_altitude = altitude;                       // Variable called user_altitude
  int photoresistor_value = analogRead(A2);              // Photoresistor value stores in variable

  if (isnan(humidity))                                   // Checks DHT22 reading
    {                                                    // If works uncorrectly displays text
      lcd.setCursor(0,2);                                // LCD cursor position
      Serial.println(F("Brak odczytu z DHT!"));
      lcd.print("BrakodczytuzDHT!");
      return;
    }
    
  button_state = digitalRead(button_pin);     // Reads button state and stores it in variable
  Serial.println(" ");
  Serial.print("  -  ");
  Serial.print("Stan przycisku:  ");
  Serial.print(button_state);                 // Prints button state (0 or 1)
  
  button = button_state;           // Button state stores in variable
  if(push==0 && button==1) 
  {
  C_or_F = !C_or_F;                // Reads state of button (0 or 1)
  }
  push = button;
  
  if (C_or_F)                      // Button state 0
    {

      digitalWrite(green_led_pin, HIGH);               // Turn on green LED
      digitalWrite(red_led_pin, LOW);                  // Turn off red LED

      
      DS18B20.requestTemperatures();                        // Request temperature value from DS18B20 sensor
      tempC = DS18B20.getTempCByIndex(0);                   // Reads temperature value in C
      stat = BMP180.startTemperature();                     // Starts to collect temperature data from BMP180 sensor
      stat = BMP180.getTemperature(t);                      // Reads temperature from BMP180 sensor
      stat = BMP180.startPressure(3);                       // Starts to collect pressure data form BMP180 sensor
      stat = BMP180.getPressure(pressure,tempC);            // Reads pressure from BMP180 sensor
      pressure0 = BMP180.sealevel(pressure, altitude);      // Reads pressure to sealevel from BMP180 sensor
      
      if(photoresistor_value > 800)                                 // Max bright value = 1000
      photoresistor_value = 1000;
      else if(photoresistor_value < 150)                            // Min bright value = 150
      photoresistor_value = 150;
      pwm_back_light=255+(photoresistor_value/2);                   // Equation for good LCD dim
      analogWrite(back_light_pin,pwm_back_light);                   // Diming LCD
      
      Serial.print("  -  ");
      Serial.print("Temperatura: ");
      Serial.print(tempC);                                    // Shows temperature in C
      Serial.print("°C");
      Serial.print("  -  ");
      Serial.print("Cisnienie: ");
      Serial.print(pressure);                                 // Shows pressure in hPa
      Serial.print("hPa");
      Serial.print("  -  ");
      Serial.print("Podana wysokosc: ");
      Serial.print(user_altitude,0);                          // Shows altitude in meters
      Serial.print(" metry");
      Serial.print("  -  ");
      Serial.print(user_altitude*3.28084,0);                  // Shows altitude in feets
      Serial.print(" stopy");
      Serial.print("  -  ");
      Serial.print("Wilgotnosc: ");
      Serial.print(humidity);                                 // Shows humidity in %
      Serial.print("%");
      Serial.print("  -  ");
      Serial.print("Wartosc natezenia swiatla: ");
      Serial.print(photoresistor_value);                      // Shows light value
  
      lcd.setCursor(0,2);                             // LCD cursor position
      lcd.print(round(tempC));                        // Shows temperature in C
      lcd.print("C  ");
      lcd.print(round(pressure));                     // Shows pressure in hPa
      lcd.print("hPa ");
      lcd.print(round(humidity));                     // Shows humidity in %
      lcd.print("%   ");
    }
   
  else  // Button state 1
    {   
      digitalWrite(green_led_pin, LOW);               // Turn off green LED
      digitalWrite(red_led_pin, HIGH);                // Turn on red LED
      
      DS18B20.requestTemperatures();                        // Request temperature value from DS18B20 sensor
      tempC = DS18B20.getTempCByIndex(0);                   // Reads temperature value in C
      tempF = tempC * 9 / 5 + 32;                           // Converts temperature value from C to F
      stat = BMP180.startTemperature();                     // Starts to collect temperature data from BMP180 sensor
      stat = BMP180.getTemperature(t);                      // Reads temperature from BMP180 sensor
      stat = BMP180.startPressure(3);                       // Starts to collect pressure data form BMP180 sensor
      stat = BMP180.getPressure(pressure, user_altitude);   // Reads pressure from BMP180 sensor
      pressure0 = BMP180.sealevel(pressure, altitude);      // Reads pressure to sealevel from BMP180 sensor

      if(photoresistor_value > 800)                                 // Max bright value = 1000
      photoresistor_value = 1000;
      else if(photoresistor_value < 150)                            // Min bright value = 150
      photoresistor_value = 150;
      pwm_back_light=255+(photoresistor_value/2);                   // Equation for good LCD dim
      analogWrite(back_light_pin,pwm_back_light);                   // Diming LCD

      Serial.print("  -  ");
      Serial.print("Temperatura: ");
      Serial.print(tempF);                                    // Shows temperature in F
      Serial.print("°F");
      Serial.print("  -  ");
      Serial.print("Cisnienie: ");
      Serial.print(pressure);                                 // Shows pressure in hPa
      Serial.print("hPa");
      Serial.print("  -  ");
      Serial.print("Podana wysokosc: ");
      Serial.print(user_altitude,0);                          // Shows altitude in meters
      Serial.print(" metry");
      Serial.print("  -  ");
      Serial.print(user_altitude*3.28084,0);                  // Shows altitude in feets
      Serial.print(" stopy");
      Serial.print("  -  ");
      Serial.print("Wilgotnosc: ");
      Serial.print(humidity);                                 // Shows humidity in %
      Serial.print("%");
      Serial.print("  -  ");
      Serial.print("Wartosc natezenia swiatla: ");
      Serial.print(photoresistor_value);                      // Shows light value
      
  
      lcd.setCursor(0,2);                             // LCD cursor position
      lcd.print(round(tempF));                        // Shows temperature in F
      lcd.print("F  ");
      lcd.print(round(pressure));                     // Shows pressure in hPa
      lcd.print("hPa ");
      lcd.print(round(humidity));                     // Shows humidity in %
      lcd.print("%   ");
   }
}
