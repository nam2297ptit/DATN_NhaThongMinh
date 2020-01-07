#include <PubSubClient.h>
#include <ESP8266WiFi.h>

#define ssid "nkt"
#define password "12345679"

#define mqtt_server "postman.cloudmqtt.com"
#define mqtt_port  14601
#define mqtt_user "xaxcxxig"    
#define mqtt_pwd "NwZIb4W_-NDU"
#define mqtt_topic_led "led"
#define ID  "LHV-01"


#define LED1 12
#define LED2 14
//#define BT1  D5

WiFiClient espClient;
PubSubClient client(espClient);


int buttonStatus = 0;
int oldButton = 0;
int ledStatus = 0;

/*----SetUp-----*/
void setup(){
  // put your setup code here, to run once:
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server,mqtt_port);

  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
//  pinMode(BT1,INPUT);
  
  digitalWrite(LED1,LOW);
  digitalWrite(LED2,LOW);

  client.setCallback(callback);
}
/*----Loop-----*/
void loop(){
   if(!client.connected()){
        reconnect();
    }
    client.loop();
// đọc trạng thái của nút nhấn nếu nút được nhấn đảo trạng thái đèn
//    buttonStatus = digitalRead(BT1);
//    if(buttonStatus != oldButton){
//      oldButton = buttonStatus;
//      delay(50);
//      if(buttonStatus == 0){ 
//        ledStatus = !ledStatus;
//        if(ledStatus == 1){ 
//          Ctl_Led(LED1,1);
//          client.publish(mqtt_topic_led,"ON1");
//        }
//        else{ 
//          Ctl_Led(LED1,0);
//          client.publish(mqtt_topic_led,"OFF1");
//          }
//      }
//   } 
}
/**
  * @brief:  fucntion callback of mqtt 
  * @param  topic: topic mqtt client subcrice to
  *         mesage: message
  *         length: len of message
  * @retval None
**/
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
    if(messageTemp.equals("ON1") == 1) Ctl_Led(LED1,1);
    if(messageTemp.equals("ON2") == 1) Ctl_Led(LED2,1);
    /*LED OFF*/
    if(messageTemp.equals("OFF1") == 1) Ctl_Led(LED1,0);
    if(messageTemp.equals("OFF2") == 1) Ctl_Led(LED2,0);
}
/**
  * @brief:  Control LEDx 
  * @param:  led is led to cotrol 
  *              where is LED1,LED2,LED3
  * @param:  Status:
  *           1: LED ON
  *           0: LED OFF
  * @retval None
**/
void Ctl_Led(int led,int Status){
  digitalWrite(led,Status == 1 ? HIGH : LOW);
}
/**
  * @brief:  Setup wifi for ESP and print IP address.
  * @param: None
  * @retval None
**/
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
/**
  * @brief:  Connect to MQTT broker with ID, user and pass
  *          and pushlish init value of LEDs
  * @param: None
  * @retval None
**/
void reconnect(){
  // Chờ tới khi kết nối
  while (!client.connected()){
    Serial.print("Attempting MQTT connection...");// Thực hiện kết nối với mqtt user và pass
    if(client.connect(ID,mqtt_user, mqtt_pwd)){
      Serial.println("connected");// Khi kết nối sẽ publish thông báo
//      client.publish(mqtt_topic_led,"ESP_reconnected");// ... và nhận lại thông tin này
      // gửi trạng thái tắt cả 3 đèn
      client.publish(mqtt_topic_led,"OFF1");
      client.publish(mqtt_topic_led,"OFF2");
      // subcrice đến topic của mỗi đèn để nhận tín hiệu điều khiển
      client.subscribe(mqtt_topic_led);
    }
    else{
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);// Đợi 5s
    }
  }
}
