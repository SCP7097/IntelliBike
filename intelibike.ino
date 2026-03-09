#include <DHT.h>
#include <SPI.h> // SPI 通信协议库（用于部分外设通信）
#include <Wire.h> // I2C 通信协议库（用于 OLED、部分传感器通信）
#include <Adafruit_GFX.h>// OLED 屏幕图形驱动库
#include <Adafruit_SSD1306.h> // SSD1306 型号 OLED 屏幕驱动库
#define Trig 4 //超声波发送端引脚
#define Echo 5//超声波接收端引脚
#define DHTPIN 6     // 温度湿度传感器引脚
#define DHTTYPE DHT11  // 传感器类型
#define D 2//设定蜂鸣器引脚
Adafruit_SSD1306 OLED(128,64,&Wire,-1);//设定荧光屏长，宽
DHT dht(DHTPIN, DHTTYPE);//设定温湿度传感器类型

void setup() {
    Serial.begin(9600);
    dht.begin();  // 初始化传感器
    pinMode(Trig, OUTPUT);
    pinMode(Echo, INPUT);
    float d=0.0;
    digitalWrite(D,LOW);
    OLED.begin(SSD1306_SWITCHCAPVCC,0x3c);
    OLED.setTextColor(WHITE);

    pinMode(D,OUTPUT);
}

void loop() {
    delay(100);  
    float T = dht.readTemperature();

    digitalWrite(Trig,LOW);
    delayMicroseconds(2);
    digitalWrite(Trig,HIGH);
    delayMicroseconds(10);
    digitalWrite(Trig,LOW);

    float t = pulseIn(Echo, HIGH);  // 往返时间（μs）

    float norm_distance = ((331.4+0.607*T)*0.000001*t)/2 * 100; //distance normalized using temperature
    float distance =t/58.0;
    Serial.print("距离: ");
    Serial.print(norm_distance);
    Serial.println(" cm");

    if (norm_distance<50.0){
        digitalWrite(D,LOW);
        delay(100);
        digitalWrite(D,HIGH);
        delay(100);}
    if (norm_distance<25){
        digitalWrite(D,LOW);
        delay(10);
        digitalWrite(D,HIGH);
        delay(10);
    }
    OLED.clearDisplay();
    OLED.setCursor(0,0);
    OLED.print(distance);
    OLED.setCursor(0,20);
    OLED.print(T);
    OLED.setCursor(0,10);
    OLED.print(distance);
    OLED.display();
}