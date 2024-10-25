/*************************************************************
  Blynk is a platform with iOS and Android apps to control
  ESP32, Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build mobile and web interfaces for any
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: https://www.blynk.io
    Sketch generator:           https://examples.blynk.cc
    Blynk community:            https://community.blynk.cc
    Follow us:                  https://www.fb.com/blynkapp
                                https://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************
  This example shows how to use Arduino WiFi shield
  to connect your project to Blynk.

  Please update your shield firmware:
    https://www.arduino.cc/en/Hacking/WiFiShieldFirmwareUpgrading

  Feel free to apply it to any other example. It's simple!
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

/* Fill in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL6QRjPH69I"
#define BLYNK_TEMPLATE_NAME "smartparking"
#define BLYNK_AUTH_TOKEN    "po6V3XG9qb7bnsUw9g21VbVpY7JBvgYo"


#include <SPI.h>
#include <WiFiS3.h>
#include <BlynkSimpleWifi.h>
#include <Servo.h>

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "NT wifi 4106-2.4G";
char pass[] = "0957959968";

int num_array[10][7] = {  
                          { 1,1,1,1,1,1,0 },    // 0
                          { 0,1,1,0,0,0,0 },    // 1
                          { 1,1,0,1,1,0,1 },    // 2
                          { 1,1,1,1,0,0,1 },    // 3
                          { 0,1,1,0,0,1,1 },    // 4
                          { 1,0,1,1,0,1,1 },    // 5
                          { 1,0,1,1,1,1,1 },    // 6
                          { 1,1,1,0,0,0,0 },    // 7
                          { 1,1,1,1,1,1,1 },    // 8
                          { 1,1,1,0,0,1,1 }};   // 9

const int segment_a = 2; 
const int segment_b = 3; 
const int segment_c = 4;
const int segment_d = 5;
const int segment_e = 6;
const int segment_f = 7;
const int segment_g = 8;
Servo gate_in;
Servo gate_out;
const int in_sensor = 12;
const int out_sensor = 13;
const int ledPin = 1;
int park_counter = 3;
int park_limit = 3;

void setup()
{
  // Debug console
  Serial.begin(9600);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  Blynk.virtualWrite(V2, park_limit);
  pinMode(ledPin, OUTPUT);          // ตั้งค่าพิน LED เป็น OUTPUT
  gate_in.attach(10);
  gate_out.attach(11);
  pinMode(in_sensor, INPUT);
  pinMode(out_sensor, INPUT);
  pinMode(segment_a, OUTPUT);   
  pinMode(segment_b, OUTPUT);   
  pinMode(segment_c, OUTPUT);   
  pinMode(segment_d, OUTPUT);   
  pinMode(segment_e, OUTPUT);   
  pinMode(segment_f, OUTPUT);   
  pinMode(segment_g, OUTPUT);
}

void loop()
{
  Blynk.run();
    Blynk.virtualWrite(V0, park_counter);

      if (!digitalRead(in_sensor) && (park_counter > 0)) {
        gate_in.write(90);
        delay(100);
        park_counter -= 1;
        displayDigit(park_counter);
        Blynk.virtualWrite(V0, park_counter);  // ส่งค่า park_counter ไปยัง Blynk
        delay(2000);
        gate_in.write(0);
        delay(100);
    }

    if (!digitalRead(out_sensor) && (park_counter < park_limit)) {
        gate_out.write(90);
        delay(1000);
        park_counter += 1;
        displayDigit(park_counter);
        Blynk.virtualWrite(V0, park_counter);  // ส่งค่า park_counter ไปยัง Blynk
        delay(2000);
        gate_out.write(0);
        delay(100);
    }

}

BLYNK_WRITE(V1) {
    int ledState = param.asInt(); // รับค่าจาก Blynk App (1 = เปิด, 0 = ปิด)
    digitalWrite(ledPin, ledState); // ควบคุมสถานะ LED ตามค่าที่ได้รับ
}

BLYNK_WRITE(V2) {
    int new_park_limit = param.asInt();  // รับค่าจากแถบเลื่อน Blynk (V2) สำหรับกำหนด park_limit ใหม่
  Serial.print("New park limit set to: ");
  Serial.println(new_park_limit);

  // ตรวจสอบจำนวนที่จอดรถ (park_limit) ที่เปลี่ยนแปลง
  if (new_park_limit > park_limit) {
    // หากเพิ่มจำนวนที่จอดใหม่ ให้ปรับเพิ่มค่า park_counter ให้สอดคล้อง
    park_counter += (new_park_limit - park_limit);
    Serial.print("Park counter increased to: ");
    Serial.println(park_counter);
  } else if (new_park_limit < park_limit) {
    // หากลดจำนวนที่จอด และปัจจุบันมีรถจอดอยู่เกิน limit ใหม่ ให้ลดค่า park_counter ให้สอดคล้อง
    if (park_counter > new_park_limit) {
      park_counter = new_park_limit;
      Serial.print("Park counter decreased to: ");
      Serial.println(park_counter);
    }
  }

  // ตั้งค่า park_limit ใหม่
  park_limit = new_park_limit;

  // ส่งค่า park_counter ไปยัง Blynk App เพื่ออัปเดตหน้าจอ
  Blynk.virtualWrite(V0, park_counter);
}

void displayDigit(int digit) {
  // Set each segment according to the num_array for the given digit
  digitalWrite(segment_a, num_array[digit][0]);
  digitalWrite(segment_b, num_array[digit][1]);
  digitalWrite(segment_c, num_array[digit][2]);
  digitalWrite(segment_d, num_array[digit][3]);
  digitalWrite(segment_e, num_array[digit][4]);
  digitalWrite(segment_f, num_array[digit][5]);
  digitalWrite(segment_g, num_array[digit][6]);
}

