#include <stdlib.h>
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

typedef struct node{
    float dist;
    struct node* next;
} node;

int max_node = 3;
node* head = NULL;

void push(node** head, float dist);
void pop_first(node** head);
int check_full(node* head, int max_node);
void print_list(node* head);
float find_median(node* head, int max_node);
int compare(const void* a, const void* b) {
    float fa = *(const float*)a, fb = *(const float*)b;
    return (fa > fb) - (fa < fb);
}

void setup() {
    Serial.begin(9600);
    dht.begin();  // init dht sensor
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
    push(&head, norm_distance);
    if(check_full(head, max_node)){
        pop_first(&head);
    }
    // for debugging
    // print_list(head);
    Serial.print("距离: ");
    Serial.print(find_median(head,max_node));
    Serial.println(" cm");
    if (norm_distance < 50.0){
        digitalWrite(D,LOW);
        delay(100);
        digitalWrite(D,HIGH);
        delay(100);}
    if (norm_distance < 25){
        digitalWrite(D,LOW);
        delay(10);
        digitalWrite(D,HIGH);
        delay(10);
    }
    OLED.clearDisplay();
    OLED.setCursor(0,0);
    OLED.print(norm_distance);
    OLED.setCursor(0,20);
    OLED.print(T);
    OLED.setCursor(0,10);
    OLED.print(norm_distance);
    OLED.display();
}

void push(node** head, float dist){
    //in this implementation for intellibike we push to the end so that in the sliding window the values are organized
    node* new_node = (node*)malloc(sizeof(node));
    new_node->dist = dist;
    new_node->next = NULL;
    if (*head == NULL){
        *head = new_node;
        return;
    }
    node* current = *head;
    while (current->next != NULL){
        current = current->next;
    }//cycle to last
    current->next = new_node;
}
void pop_first(node** head) {
    if (*head == NULL) return;
    node* old_head = *head;
    *head = (*head)->next;
    free(old_head);
}
int check_full(node* head, int max_node){
    node* current = head;
    int nnode = 0;
    while (current != NULL){
        nnode++;
        current = current->next;
    }
    if (nnode > max_node){
        return 1;
    }
    return 0;
}
void print_list(node* head){
    node* current = head;
    while(current != NULL){
        printf("%f meters\n", current->dist);
        current = current->next;
    }
}

float find_median(node* head, int max_node){
    if (head == NULL){
        return 0.0f; // should prob fix this but not sure how rendering works
    }
    float* distp = malloc(max_node* sizeof(float));
    node* current = head;
    int n = 0;
    // create list
    while(current != NULL){
        distp[n] = current->dist;
        n++;
        current = current->next;
    }
    qsort(distp,n,sizeof(float), compare);
    float median;
    if (n % 2 != 0) {
        median = distp[n / 2];
    } else {
        median = (distp[n / 2 - 1] + distp[n / 2]) / 2.0f;
    }
    free(distp);
    return median;
}