#include <SoftwareSerial.h>
#include <dht11.h>

SoftwareSerial GSM(7, 8);

#define GAS_PIN 3
#define DHT_PIN 11
#define RAIN_PIN 12
#define SOIL_PIN A2
// This is for if you want an alert function
//#define TOUCH_PIN A3

#define FAN_PIN 5  
#define BULB_PIN 6
#define PUMP_PIN 9
#define BUZZER_PIN 10

#define DHT11PIN DHT_PIN


dht11 DHT11;
String msg;

String FanState = "off";
String BulbState = "off";
String PumpState = "off";
String BuzzerState = "off";
String RainState = "Low";
String GasState = "Low";


// This same as the key you enter the mobile app security key
String Security = "%$&23Wr";

//int TouchValue = 0;  

void setup() {

  GSM.begin(9600);
  Serial.begin(9600);
 
  // Devices
  pinMode(FAN_PIN, OUTPUT);
  pinMode(BULB_PIN, OUTPUT);
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(FAN_PIN, HIGH);
  digitalWrite(BULB_PIN, HIGH);
  digitalWrite(PUMP_PIN, HIGH);
  digitalWrite(BUZZER_PIN, HIGH);
  
  // Sensors
  pinMode(DHT_PIN, INPUT);
  pinMode(SOIL_PIN, INPUT);
  pinMode(GAS_PIN, INPUT);
  pinMode(TOUCH_PIN, INPUT);
  pinMode(RAIN_PIN, INPUT);


  // Set SMS mode to text
  GSM.println("AT+CMGF=1");
  delay(1000);

  // Set to show SMS messages on serial
  GSM.println("AT+CNMI=1,2,0,0,0");
  delay(1000);

}


void loop() {

  if (GSM.available()) {
    char c = GSM.read();
    msg += c;
    Serial.write(c);

    // Check if the message ends with a newline character
    if (c == '\n') {
      processIncomingMessage(msg);
      msg = "";
    }
  }  
  
  
  if (Serial.available()) {
   GSM.write(Serial.read());
  }
  

  // If you want alert-type messages you can add a function like this way
  // Touch();

}



void processIncomingMessage(String message) {

  Serial.println("new message:");
  Serial.println(message);
  

  // Further processing of the message here


  // Bulb ON / OFF
  if (msg.indexOf(Security + "Bulb_on") >= 0) {
    digitalWrite(BULB_PIN, LOW);
    msg = "";
    BulbState = "on";
  }


  if (msg.indexOf(Security + "Bulb_off") >= 0) {
    digitalWrite(BULB_PIN, HIGH);
    msg = "";
    BulbState = "off";
  }



  // Fan ON / OFF
  if (msg.indexOf(Security + "Fan_on") >= 0) {
    digitalWrite(FAN_PIN, LOW);
    msg = "";
    FanState = "on";
  }

  if (msg.indexOf(Security + "Fan_off") >= 0) {
    digitalWrite(FAN_PIN, HIGH);
    msg = "";
    FanState = "off";
  }


  // Water ON / OFF
  if (msg.indexOf(Security + "Water_on") >= 0) {
    digitalWrite(PUMP_PIN, LOW);
    msg = "";
    PumpState = "on";
  }


  if (msg.indexOf(Security + "Water_off") >= 0) {
    digitalWrite(PUMP_PIN, HIGH);
    msg = "";
    PumpState = "off";
  }



  // Buzzer ON / OFF
  if (msg.indexOf(Security + "Buzzer_on") >= 0) {
    digitalWrite(BUZZER_PIN, LOW);
    msg = "";
    BuzzerState = "on";
  }


  if (msg.indexOf(Security + "Buzzer_off") >= 0) {
    digitalWrite(BUZZER_PIN, HIGH);
    msg = "";
    BuzzerState = "off";
  }


  // Devices update section

  if (msg.indexOf(Security + "Devices_report") >= 0) {

    Serial.println("Sending Message");
    GSM.println("AT+CMGF=1");
    delay(1000);
    GSM.println("AT+CMGS=\"+9476XXXXXXX\"\r"); // replace your number with country code
    delay(1000);
    Serial.println("Set SMS Content");
    GSM.println("_Fan:" + FanState + "_Bulb:" + BulbState + "_Bulb 2:" + PumpState + "_Alarm:" + BuzzerState + "_");   //  GSM.println("_Bulb:" + BulbState + "_");   // If you want a single device
    delay(100);
    Serial.println("Finish");
    GSM.println((char)26);
    delay(1000);
    Serial.println("Message has been sent");
    msg = "";

  }

  // Sensors update section

  if (msg.indexOf(Security + "Sensors_report") >= 0) {
    
    ///// First, read sensor values /////

    // Temperature sensor
    int chk = DHT11.read(DHT11PIN);
    float humidity = (float)DHT11.humidity;
    float temperature = (float)DHT11.temperature;

    // Rain sensor
    int RainValue = digitalRead(RAIN_PIN);
    Serial.println("Rain Value: " + String(RainValue));

    if (RainValue == HIGH) {
    RainState = "Low";
    Serial.println("no rain");
    } else {
    Serial.println("rain");
    RainState = "High";
    }
    
    // Gas sensor
    int GasValue = digitalRead(GAS_PIN);
    Serial.println("Gas: " + String(GasValue));

    if (GasValue == HIGH) {
    GasState = "Low";
    Serial.println("no Gas");
    } 
    
    else {
    Serial.println("Gas here");
    GasState = "High";
    }

    // Soil sensor
    int SoilValue = analogRead(SOIL_PIN);
    int SoilOutputValue = map(SoilValue, 0, 1023, 100, 0);




    ///// Next send sensor values /////

    delay(500);
    Serial.println("Sending Message");
    GSM.println("AT+CMGF=1");
    delay(1000);
    GSM.println("AT+CMGS=\"+9476XXXXXXX\"\r"); // replace your number with country code
    delay(1000);
    Serial.println("Set SMS Content");
    GSM.println("_Humidity:" + String(humidity) + "_Temperature:" + String(temperature) + "_Rain:" + RainState + "_Gas:" + GasState + "_Soil:" + String(SoilOutputValue) + "_");  //  GSM.println("_Temperature:" + String(temperature) + "_");  // If you want a single sensor
    delay(100);
    Serial.println("Finish");
    GSM.println((char)26);
    delay(1000);
    Serial.println("Message has been sent");
    msg = "";

  }

}



// Alert action (if you want)


void Touch()

{
  
  TouchValue = digitalRead(TOUCH_PIN);   

 
 if (TouchValue == HIGH)   
 {           
  SendAlert(); 

 }   else{}

}
 



 void SendAlert() {
  Serial.println("Sending Alert");
  GSM.println("AT+CMGF=1"); // Sets the GSM Module in Text Mode
  delay(1000);
  Serial.println("Set SMS Number");
  GSM.print("AT+CMGS=\"+91XXXXXXXX\"\r"); // Replace with your mobile number
  delay(100);
  GSM.print("Alert..!"); // Message content
  delay(100);
  GSM.print((char)26); 
  delay(100);
  Serial.println("Alert has been sent");
}