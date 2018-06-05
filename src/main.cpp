#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <math.h>

#define ADDR 8
#define CHARA 4

uint8_t pwmpin = 5;
unsigned int mangle, mask = 0b00111111111, angle;
unsigned long time, prtime;
bool sout = false;

void print_transmit(char *transmit)
{
  Serial.println(" ");
  for (int i=0; i<CHARA-1; i++)
  {
    if (transmit[i] == 0)
      break;
    Serial.print((int)transmit[i], BIN);
    Serial.print(" ");
  }
  Serial.println("");
}

void requestEvent(void)
{
  char transmit[CHARA];
  itoa(angle, transmit, 10);

  if (sout)
    print_transmit(transmit);
  Wire.write(transmit);
}

void requestEvent2(void)
{
  if (sout)
    Serial.write("%d\n", angle);
  Wire.write(angle);
}

void probe_angle(unsigned int *angle)
{
  SPI.beginTransaction(SPISettings(140000, MSBFIRST, SPI_MODE2));
  mangle = SPI.transfer16(0x0000);
  *angle = (mangle>>6) & mask;
  SPI.endTransaction();
}

void print_angle(unsigned int *angle)
{
  time = millis();
  if (time > prtime + 20000)
  {
    prtime = time;
    Serial.print("value: \t");
    Serial.print(*angle, BIN);
    Serial.print(", millis: ");
    Serial.print(time);
    Serial.print(", angle: ");
    Serial.println(*angle * 2 * PI / 512,3);
  }
}

void setup() {
    // set timer 0 divisor to 1
    // for PWM frequency of 62500.00 Hz
    TCCR0B = (TCCR0B & B11111000) | B00000001;

    Wire.begin(ADDR);
    digitalWrite(3, HIGH);
    Wire.onRequest(requestEvent);
    SPI.begin();
    if (sout)
    {
      Serial.begin(9600);
      Serial.println("I2C Angle");
    }
}

void loop() {
    delay(1);
    probe_angle(&angle);
    if (sout)
      print_angle(&angle);
    analogWrite(pwmpin, angle/2);
}
