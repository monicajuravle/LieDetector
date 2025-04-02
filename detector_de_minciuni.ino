#include <PulseSensorPlayground.h>
#include <LiquidCrystal.h>
#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal lcd(12, 11, 7, 6, 5, 4);

const int PulseWire = 0;
const int LED = LED_BUILTIN;
int Threshold = 522;
const int LED_BUZZER=8;

PulseSensorPlayground pulseSensor;

unsigned long previousMillis = 0;
const long interval = 2000;
float readingCounts=false;
float average=62.0;

const int QUEUE_SIZE = 10;  // Size of the queue for BPM readings
int bpmQueue[QUEUE_SIZE];  // Array to store BPM readings
int index=0;

void setup() {
  pinMode(8, OUTPUT);
  Serial.begin(115200);

  dht.begin();
  lcd.begin(16, 2);

  pulseSensor.analogInput(PulseWire);
  pulseSensor.blinkOnPulse(LED);
  pulseSensor.setThreshold(Threshold);

  if (pulseSensor.begin()) {
    Serial.println("We created a pulseSensor Object !");
  }
}

void loop() {


  int rawSignal = analogRead(PulseWire);
  Serial.println(String(rawSignal));
  
  

  if (pulseSensor.sawStartOfBeat()) {
    lcd.setCursor(0, 0);
    int myBPM = pulseSensor.getBeatsPerMinute();
    if(readingCounts)
    {
        float average=calculate_average();
        //Serial.println(String(average));
    }
    if(myBPM-average>10)
    {
      
      digitalWrite(LED_BUZZER,HIGH);
      delay(100);
      digitalWrite(LED_BUZZER,LOW);

    }
    else
    {
      digitalWrite(LED_BUZZER,LOW);
      bpmQueue[index]=myBPM;
      index++;
      if(index>9)
      {
        readingCounts=true;
        index++;
      }
    }

    lcd.print("♥ BPM:" + String(myBPM));
  }

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
  lcd.setCursor(0, 1);
  float humi = dht.readHumidity();
  float tempC = dht.readTemperature();

  if (isnan(humi) || isnan(tempC)) {
    lcd.print("Failed to read from DHT sensor!");
  } else {
    lcd.print("H:" + String(humi) + " C:" + String(tempC));
  }
  }

  //clear lcd
  delay(20);
}

float calculate_average()
{
  float avg=0.0;
  for(int i=0;i<10;i++)
  {
    avg+=bpmQueue[i];
  }
  return avg/10;
}