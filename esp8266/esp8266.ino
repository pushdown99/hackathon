#include <IotWebConf.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

#define WITH_ESP12      // WITH_ESP12 | WITH_ESP32

#if defined(WITH_ESP32)
#include <HTTPClient.h>
#else
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#endif

const char dev[]  = "popup-iot";
const char pwd[]  = "1234567890";

const char host[]   = "http://www.tric.kr/popup-iot/";
const char shost[]  = "https://dweet.io:443/dweet/for/popup-iot";

const char mqttsvr[]  = "m16.cloudmqtt.com";
const int  mqttport   = 11967;
const char mqttid[]   = "ctgvhlad";
const char mqttpwd[]  = "gk-mS1PQg73a";
const char mqttch[]   = "esp/test";

#define WITH_MQTT   1
#define WITH_HTTP   2
#define WITH_HTTPS  3

int method = WITH_HTTP;

DNSServer dnsServer;
WebServer server(80);
IotWebConf iotWebConf(dev, &dnsServer, &server, pwd);

WiFiClient client;
PubSubClient mqtt(client);

#if defined(WITH_ESP32)
int En0 = 25; //  Low enabled
int En1 = 26; //  Low enabled
int S0  = 27;
int S1  = 14;
int S2  = 12;
int S3  = 13; 
int SP  = 34;
#else
int En0 = 10; //  Low enabled
int En1 = 5;  //  Low enabled
int S0  = 4;
int S1  = 0;
int S2  = 2;
int S3  = 14; 
int SP  = A0;
#endif

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Starting up...");

  iotWebConf.init();
  server.on("/", handleRoot);
  server.on("/config", []{ iotWebConf.handleConfig(); });
  server.onNotFound([](){ iotWebConf.handleNotFound(); });
  Serial.println("Ready.");

  pinMode(En0, OUTPUT);
  pinMode(En1, OUTPUT);
  pinMode(S0,  OUTPUT);
  pinMode(S1,  OUTPUT);
  pinMode(S2,  OUTPUT);
  pinMode(S3,  OUTPUT);

  if ( method == WITH_MQTT ) mqtt.setServer(mqttsvr, mqttport);
}

void loop() {
  iotWebConf.doLoop();
  if( WiFi.status() == WL_CONNECTED ) dataHandle();
  delay(2000);
}

//////////////////////////////////////////////////////////////////////////////

void handleRoot() {
  if (iotWebConf.handleCaptivePortal())
  {
    return;
  }
  String s = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/>";
  s += "<title>IotWebConf 01 Minimal</title></head><body>Hello world!";
  s += "Go to <a href='config'>configure page</a> to change settings.";
  s += "</body></html>\n";
  server.send(200, "text/html", s);
}

void initMqtt() {
  Serial.println("Conntected to mqtt network..");
  while (!mqtt.connected()) {
    Serial.println("Connecting to MQTT...");
    if (mqtt.connect(dev, mqttid, mqttpwd )) {
      Serial.println("Connected.");  
    } else {
      Serial.print(mqtt.state());
      Serial.print("Failed with state.. try again in 2 seconds");
      delay(2000);
    }
  }
}

void pubMqtt(char *buf) {
  Serial.println(buf);
  mqtt.publish(mqttch, buf, true);
}

void postHttp(char *buf) {
  HTTPClient http;
  Serial.print("Conntecting to server.. ");
  Serial.println(host);
  if (http.begin(host)) {
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST(buf);
    Serial.println(httpCode);
    if (httpCode == HTTP_CODE_OK) {
      Serial.println(http.getString());
    } 
  } else {
    Serial.print("Failed to connect to server");
  }
}

void postHttps(char *buf) {
#if defined(WITH_ESP12)
  BearSSL::WiFiClientSecure client;
  client.setInsecure();
#else
  const char* root_ca= \
  "-----BEGIN CERTIFICATE-----\n" \
  "MIICiTCCAg+gAwIBAgIQH0evqmIAcFBUTAGem2OZKjAKBggqhkjOPQQDAzCBhTEL\n" \
  "MAkGA1UEBhMCR0IxGzAZBgNVBAgTEkdyZWF0ZXIgTWFuY2hlc3RlcjEQMA4GA1UE\n" \
  "BxMHU2FsZm9yZDEaMBgGA1UEChMRQ09NT0RPIENBIExpbWl0ZWQxKzApBgNVBAMT\n" \
  "IkNPTU9ETyBFQ0MgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMDgwMzA2MDAw\n" \
  "MDAwWhcNMzgwMTE4MjM1OTU5WjCBhTELMAkGA1UEBhMCR0IxGzAZBgNVBAgTEkdy\n" \
  "ZWF0ZXIgTWFuY2hlc3RlcjEQMA4GA1UEBxMHU2FsZm9yZDEaMBgGA1UEChMRQ09N\n" \
  "T0RPIENBIExpbWl0ZWQxKzApBgNVBAMTIkNPTU9ETyBFQ0MgQ2VydGlmaWNhdGlv\n" \
  "biBBdXRob3JpdHkwdjAQBgcqhkjOPQIBBgUrgQQAIgNiAAQDR3svdcmCFYX7deSR\n" \
  "FtSrYpn1PlILBs5BAH+X4QokPB0BBO490o0JlwzgdeT6+3eKKvUDYEs2ixYjFq0J\n" \
  "cfRK9ChQtP6IHG4/bC8vCVlbpVsLM5niwz2J+Wos77LTBumjQjBAMB0GA1UdDgQW\n" \
  "BBR1cacZSBm8nZ3qQUfflMRId5nTeTAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0TAQH/\n" \
  "BAUwAwEB/zAKBggqhkjOPQQDAwNoADBlAjEA7wNbeqy3eApyt4jf/7VGFAkK+qDm\n" \
  "fQjGGoe9GKhzvSbKYAydzpmfz1wPMOG+FDHqAjAU9JM8SaczepBGR7NjfRObTrdv\n" \
  "GDeAU/7dIOA1mjbRxwG55tzd8/8dLDoWV9mSOdY=\n" \
  "-----END CERTIFICATE-----\n";
#endif

  HTTPClient https;
  Serial.print("Conntecting to server.. ");
  Serial.println(shost);

#if defined(WITH_ESP12)
  if (https.begin(client, shost)) {
#else
  if (https.begin(shost, root_ca)) {
#endif
    https.addHeader("Content-Type", "application/json");
    int httpsCode = https.POST(buf);
    Serial.println(httpsCode);
    if (httpsCode == HTTP_CODE_OK) {
      Serial.println(https.getString());
    } 
  } else {
    Serial.print("Failed to connect to server");
  }
}

#define MAXCH 31

char* getData(char *buf, int len) {
  int controlPin[] = {S0,S1,S2,S3,En0,En1};
  int muxChannel[MAXCH][6]={
    {0,0,0,0,0,1}, //channel 0
    {0,0,0,1,0,1}, //channel 1
    {0,0,1,0,0,1}, //channel 2
    {0,0,1,1,0,1}, //channel 3
    {0,1,0,0,0,1}, //channel 4
    {0,1,0,1,0,1}, //channel 5
    {0,1,1,0,0,1}, //channel 6
    {0,1,1,1,0,1}, //channel 7
    {1,0,0,0,0,1}, //channel 8
    {1,0,0,1,0,1}, //channel 9
    {1,0,1,0,0,1}, //channel 10
    {1,0,1,1,0,1}, //channel 11
    {1,1,0,0,0,1}, //channel 12
    {1,1,0,1,0,1}, //channel 13
    {1,1,1,0,0,1}, //channel 14
    {1,1,1,1,0,1}, //channel 15
    {0,0,0,0,1,0}, //channel 16
    {0,0,0,1,1,0}, //channel 17
    {0,0,1,0,1,0}, //channel 18
    {0,0,1,1,1,0}, //channel 19
    {0,1,0,0,1,0}, //channel 20
    {0,1,0,1,1,0}, //channel 21
    {0,1,1,0,1,0}, //channel 22
    {0,1,1,1,1,0}, //channel 23
    {1,0,0,0,1,0}, //channel 24
    {1,0,0,1,1,0}, //channel 25
    {1,0,1,0,1,0}, //channel 26
    {1,0,1,1,1,0}, //channel 27
    {1,1,0,0,1,0}, //channel 28
    {1,1,0,1,1,0}, //channel 29
    {1,1,1,0,1,0}  //channel 30
  };

  const size_t capacity = JSON_ARRAY_SIZE(31) + JSON_OBJECT_SIZE(2) + 30;
  StaticJsonDocument<capacity> doc;
  
  doc["uuid"]   = dev;  
  JsonArray data = doc.createNestedArray("data");

  for(int channel=0; channel< MAXCH; channel++) {
    for(int i=0; i<6; i++) {
      digitalWrite(controlPin[i], muxChannel[channel][i]);
    }
    int val = (int)analogRead(SP);
    val = (val < 10)? 0: val;
    data.add((int)(method == WITH_MQTT)? val/10:val );
    //data.add((int)analogRead(SP));
  }
  serializeJson(doc, buf, len);
  Serial.println(buf);
  return buf;
}

#define BUFLEN  512
void dataHandle() {
  char buf[BUFLEN];
  
  switch(method) {
  case WITH_MQTT:
    if(!mqtt.connected()) initMqtt();
    pubMqtt(getData(buf, BUFLEN));
    break;
  case WITH_HTTP:
    postHttp(getData(buf, BUFLEN));
    break;
  case WITH_HTTPS:
    postHttps(getData(buf, BUFLEN));
    break;
  }
}
