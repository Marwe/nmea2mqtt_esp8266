/* based on example code of the used libs
https://github.com/knolleary/pubsubclient/blob/master/examples/mqtt_esp8266/mqtt_esp8266.ino
https://github.com/mikalhart/TinyGPSPlus/blob/master/examples/FullExample/FullExample.ino
*/
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
// The TinyGPS++ object
TinyGPSPlus gps;



// **** CONFIG SECTION BEGIN

// ****Variables
const char* ssid = "freifunk.karlsruhe.de";
const char* password = "";
const char* mqtt_server = "mqtt.machen.click";

static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 4800;


// **** custom messages you are interested in
TinyGPSCustom WindDirection(gps, "WIMWV", 1);
TinyGPSCustom WindSpeed(gps, "WIMWV", 3);


// **** CONFIG SECTION END

WiFiClient wifiClient;
PubSubClient psclient(mqtt_server, 1883, callback, wifiClient);
long lastpsmsg = 0;
char psmsg[50];
float psvalue = 0;


// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

String clientName;


void setup()
{
  // Initialize the BUILTIN_LED pin as an output
  pinMode(BUILTIN_LED, OUTPUT);
  // init builtin Serial (USB2serial)
  Serial.begin(115200);
  delay(10);
  // init NMEA (soft) serial
  ss.begin(GPSBaud);
  delay(10);
  clientName = "esp8266-";
  clientName += String(ESP.getChipId());

  Serial.println(F("NMEA2MQTT, passing on sentences via (USB2)serial"));
  Serial.println(clientName);

  // init Wifi
  setup_wifi();
  psclient.setServer(mqtt_server, 1883);
  psclient.setCallback(callback);

}

void loop()
{
  String tmptopic;
  String topic = "nmea2mqtt/";
  topic += clientName;
  topic += "/";
  if (!client.connected()) {
    reconnect();
  }
  smartDelay(50);

  tmptopic=topic+String("debug");
  client.publish(tmptopic.c_str(), msg);
  client.loop();
  if (WindDirection.isUpdated()){
    topic
    client.publish(topic, WindDirection.value());

  }


}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!psclient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (psclient.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      psclient.publish("outTopic", "hello world");
      // ... and resubscribe
      psclient.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(psclient.state());
      Serial.println(" try again in 5 seconds");
      // Wait before retrying
      smartDelay(500);
    }
  }
}

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}


static void publishMsg(topic, )
{
  int slen = strlen(str);
  for (int i=0; i<len; ++i)
    Serial.print(i<slen ? str[i] : ' ');
  smartDelay(0);
}
