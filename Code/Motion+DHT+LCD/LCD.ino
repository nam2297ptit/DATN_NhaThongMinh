#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);  

#define DHTTYPE DHT11   // DHT 11
const int DHTPin = D5;       //Đọc dữ liệu từ DHT11 
DHT dht(DHTPin, DHTTYPE); // Thiet Lap cam bien DHT.

//Motion
int ledPin = D6;        //Chân LED Arduino
int inputPin = D8;       //Chân tín hiệu cảm biến PIR
int pirState = LOW;     //Bắt đầu với không có báo động
int val = 0;

// Thông số về wifi
#define ssid "HA"
#define password "12345679"

// Thông tin về MQTT Broker
#define mqtt_server "postman.cloudmqtt.com" // Thay bằng địa chỉ IP 
#define mqtt_topic_pub "motion"   //Giữ nguyên nếu bạn tạo topic tên là demo
#define mqtt_topic_sub "motion"
#define mqtt_user "xaxcxxig"    //Giữ nguyên nếu bạn tạo user là lophocvui và pass là 123456
#define mqtt_pwd "NwZIb4W_-NDU"
const uint16_t mqtt_port = 14601; //Port của MQTT
 
WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;
// Timers auxiliar variables
long now = millis();
long lastMeasure = 0;
int check = 0;

void setup(){
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // tắt LED 
  pinMode(inputPin, INPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback); 
  dht.begin();
  // initialize LCD
  lcd.begin();
  // turn on LCD backlight                      
  lcd.backlight();
}

// Hàm kết nối wifi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


// Hàm reconnect thực hiện kết nối lại khi mất kết nối với MQTT Broker
void reconnect() {
  // Chờ tới khi kết nối
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Thực hiện kết nối với mqtt user và pass
    if (client.connect("ESP8266Client",mqtt_user, mqtt_pwd)) {
      Serial.println("connected");
      // Khi kết nối sẽ publish thông báo
      client.publish(mqtt_topic_pub, "ESP_reconnected");
      // ... và nhận lại thông tin này
      client.subscribe(mqtt_topic_sub);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Đợi 5s
      delay(5000);
    }
  }
}

void callback(String topic, byte* message, unsigned int length) {
    Serial.print("Message arrived on topic: ");
    String Topic = topic;
    Serial.print(Topic);
    Serial.print(". Message: ");
    String messageTemp;
    for(int i = 0; i < length; i++){
        //Serial.print((char)message[i]);
        messageTemp += (char)message[i];
    }
    Serial.println(messageTemp);
    /*LED ON*/
    if(messageTemp.equals("motion_on") == 1) check = 1;
    if(messageTemp.equals("motion_off") == 1) {
      check = 0;
    }
}


void PIR(){
  val = digitalRead(inputPin);    //đọc giá trị đầu vào.
  if (val == HIGH)                //nếu giá trị ở mức cao.(HIGH - 1)
  {
    digitalWrite(ledPin, HIGH); // bật LED 
    if (pirState == LOW)        
    {
      Serial.println("Motion detected!");
      pirState = HIGH;
      client.publish(mqtt_topic_pub,"theft");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.printf("   Canh bao  ");
      lcd.setCursor(0, 1);
      lcd.printf("Co ke dot nhap");
    }
  }
  else{
    digitalWrite(ledPin, LOW);
    if (pirState == HIGH)
    {
      Serial.println("Motion ended!");
      pirState = LOW;
    }
  }
}
void ReadDHT(){
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    if (isnan(h) || isnan(t)) {
      Serial.println("\nFailed to read from DHT sensor!");
      lcd.setCursor(0, 0);
      lcd.printf("  Cannot read ");
      lcd.setCursor(0, 1);
      lcd.printf("  DHT sensor ");
      return;
    }
    static char temperatureTemp[7];
    dtostrf(t, 3, 0, temperatureTemp);
    
    static char humidityTemp[7];
    dtostrf(h, 6, 0, humidityTemp);
  
    lcd.setCursor(0, 0);
    lcd.printf(" Nhiet do:%s\337C",temperatureTemp);
    lcd.setCursor(0, 1);
    lcd.printf(" Do am:%s",humidityTemp);
    lcd.print("%"); 
}
void loop(){
  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
    client.connect("ESP8266Client");
  now = millis();
  if (now - lastMeasure > 1000) {
    lastMeasure = now;
  if(check ==1){
    PIR();
  } 
  else{
    ReadDHT();
  }
}
}
