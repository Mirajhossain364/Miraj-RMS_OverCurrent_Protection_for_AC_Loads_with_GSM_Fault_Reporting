#include <SoftwareSerial.h>

SoftwareSerial SIM900A(10,11);

const int mainsensor = A1;
const int branchsensor = A2;
const int r1 = 5;
const int r2 = 6;
const int led = 7;

const float sensitivity20A = 0.100;
const int adc_mid = 512;
const float Vref = 5.0;

float readCurrent(int sensorPin, float sensitivity, int samples = 1000) {
  long sum = 0;
  for (int i = 0; i < samples; i++) {
    int adcValue = analogRead(sensorPin);
    int centered = adcValue - adc_mid;
    sum += (long)centered * (long)centered;
  }
  float mean = sum / (float)samples;
  float voltage_rms = sqrt(mean) * (Vref / 1024.0);
  float current = voltage_rms / sensitivity;
  return current;
}

void setup() {
  Serial.begin(9600);
  SIM900A.begin(9600);
  Serial.println("SIM900A Ready");
  delay(100);
  pinMode(r1, OUTPUT);
  pinMode(r2, OUTPUT);
  pinMode(led, OUTPUT);
  digitalWrite(r1, HIGH);
  digitalWrite(r2, HIGH);
  digitalWrite(led, LOW);
}

void loop() {
  float current1 = readCurrent(mainsensor, sensitivity20A, 5000);
  float current2 = readCurrent(branchsensor, sensitivity20A, 5000);

  Serial.print("Sensor1 Current = ");
  Serial.print(current1, 3);
  Serial.print(" A       ");
  Serial.print("Sensor2 Current = ");
  Serial.print(current2, 3);
  Serial.println(" A");

  if (current1 >= 3.0) {
    digitalWrite(r1, LOW);
    Serial.println("Overload on Mains! Relay OFF");
    SendMessage();
    digitalWrite(led, LOW);
  }
  if (current1 >= 2.5 && current1 < 3) {
    digitalWrite(led, HIGH);
  } else {
    digitalWrite(led, LOW);
  }
}

void SendMessage() {
  Serial.println("Sending Message");
  SIM900A.println("AT+CMGF=1");
  delay(1000);
  Serial.println("Set SMS Number");
  SIM900A.println("AT+CMGS=\"+8801710905279\"\r");
  delay(1000);
  Serial.println("Set SMS Content");
  SIM900A.println("System Blackout, The Fault detect overload");
  delay(100);
  Serial.println("Finish");
  SIM900A.println((char)26);
  delay(1000);
  Serial.println("Message has been sent ->SMS Selesai dikirim");
}
