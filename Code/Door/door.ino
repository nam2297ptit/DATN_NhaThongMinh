#include <PubSubClient.h>
#include <ESP8266WiFi.h>

#define WIFI_AP "lophocvui.com 2.4GHz"
#define WIFI_PASSWORD "88889999"

WiFiClient wifiClient;

int status = WL_IDLE_STATUS;

PubSubClient client(wifiClient);

#define mqtt_server "postman.cloudmqtt.com"
const uint16_t mqtt_port =  14601;
#define mqtt_user "xaxcxxig"
#define mqtt_pass "NwZIb4W_-NDU"

#define IN1 D1
#define IN2 D2
#define IN3 D3
#define IN4 D4
#define ENA D0
#define ENB D5

#define ENC D8
#define IN5 D7
#define IN6 D6
void setup(){
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  pinMode(IN5,OUTPUT);
  pinMode(IN6,OUTPUT);
  pinMode(ENA,OUTPUT);
  pinMode(ENB,OUTPUT);
  pinMode(ENC,OUTPUT);
  digitalWrite(ENA,HIGH);
  digitalWrite(ENB,HIGH);
  digitalWrite(ENC,HIGH);
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
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
   // dieu khien cua chinh
    if(Topic.equals("livingroom-door")){
      if(messageTemp.equals("open")){
          Door_Control(1);
      }
      else if(messageTemp.equals("closed")){
            Door_Control(0);
          }
          else{
            Stop_Door();
          }
    }
    //dieu khien  cua gara
   if(Topic.equals("garage-door")){
        if(messageTemp.equals("open")){
            Gara_Control(0);
          }
        else if(messageTemp.equals("closed")){
            Gara_Control(1);
          }
          else{
            Stop_gara();
          }
    }

    // dieu khienr cua so phong ngu
    if(Topic.equals("bedroom-window")){
        if(messageTemp.equals("open")){
            Window_Control(0);
          }
        else if(messageTemp.equals("closed")){
            Window_Control(1);
          }
          else{
            Stop_Window();
          }
    }
    
}
void Stop_gara(){
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,LOW);
}
void Stop_Door(){
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);
}
void Stop_Window(){
  digitalWrite(IN6,LOW);
  digitalWrite(IN5,LOW);
}
void Door_Control(unsigned int dir){
  if(dir == 0){
      digitalWrite(IN1,HIGH);
      digitalWrite(IN2,LOW);
    }
  else{
      digitalWrite(IN1,LOW);
      digitalWrite(IN2,HIGH); 
    }
  }
void Gara_Control(unsigned int dir){
  if(dir == 1){
      digitalWrite(IN3,HIGH);
      digitalWrite(IN4,LOW);
    }
  else{
      digitalWrite(IN4,HIGH);
      digitalWrite(IN3,LOW); 
    }
  }
void Window_Control(unsigned int dir){
  if(dir == 1){
      digitalWrite(IN5,HIGH);
      digitalWrite(IN6,LOW);
    }
  else{
      digitalWrite(IN6,HIGH);
      digitalWrite(IN5,LOW); 
    }
  }
void loop(){
   if(!client.connected()){
        reconnect();
    }
    client.loop();
}
void InitWiFi(){
    Serial.println("Connecting to WIFI");
    while (WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected to WIFI");
}
void reconnect(){
    while (!client.connected()){
    status = WiFi.status();
    if( status != WL_CONNECTED){
        WiFi.begin(WIFI_AP, WIFI_PASSWORD);
        while (WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
      }
    Serial.println("Connected to WIFI");
    }
    Serial.print("Connecting to Hass");
    if(client.connect("ESP8266 Device", mqtt_user, mqtt_pass)){
        Serial.println( "[DONE]" );
        client.subscribe("livingroom-door");
        client.subscribe("garage-door");
        client.subscribe("bedroom-window");
    } 
    else{
        Serial.print( "[FAILED] [ rc = " );
        Serial.print( client.state());
        Serial.println(" : retrying in 5 seconds]");
        delay( 5000 );
    }
  }
}
