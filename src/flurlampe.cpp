#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <PubSubClient.h>
#include <TickerScheduler.h>

#include "led.hpp"

#include "../../wlan.hpp"

#define LEDPIN 16

void setup();
void loop();
void update_leds(void*);
void update_mqtt_status(void*);

void reconnect_wifi();
void callback(char* topic, byte* payload, unsigned int length);

const char* ssid = MAKELIGHT_SSID;
const char* password = MAKELIGHT_PASS;
const char* mqtt_server = MAKELIGHT_MQTT_SERVER;

ESP8266WiFiMulti wifiMulti;

// create MQTT client
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

// Ticker to update brightness
TickerScheduler ticker(5);

// LED Stuff
long int std_step = 10;
CLed_fade blue_led(LEDPIN);

// setup LED and Client
void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output

	// Serial Stuff
	Serial.begin(9600);
	Serial.setDebugOutput(true);
	Serial.println("Start...");
	delay(10);
	// LED Stuff
	CLed_fade::brightness_t blue_brightness;
	// start value
	blue_brightness.val = 2;
	blue_brightness.max = 175;
	blue_brightness.min = blue_brightness.val;
	blue_brightness.fadeAmount = 1;
	blue_led.setUp(blue_brightness);
	blue_led.switch_on();
	Serial.println("LED initialized");
	// initialize ticker callbacks
	ticker.add(0, 10, update_leds, 0);
	ticker.add(1, 1000, update_mqtt_status, 0);

	// setup networking stuff
	wifiMulti.addAP(ssid, password);
	client.setServer(mqtt_server, 1883);
	client.setCallback(callback);
}

void callback(char* topic, byte* payload, size_t length) {
	// Switch on the LED if an 1 was received as first character
	if ((char)payload[0] == '0')
	{
		blue_led.switch_off();
	} 
	else 
	{
		blue_led.switch_on();
	}
	Serial.println(topic);
}

void reconnect() 
{
	Serial.print("Attempting MQTT connection...");
	// Attempt to connect
	if (client.connect("flurblume")) 
	{
		Serial.println("connected");
		// Once connected, publish an announcement...
		client.publish("/home/flur/flurblume/status", String(blue_led.isOn()).c_str() );
		// ... and resubscribe
		client.subscribe("/home/flur/command");
		client.subscribe("/home/flur/flurblume/command");
		digitalWrite(LED_BUILTIN, HIGH);
	} 
	else
	{
		//digitalWrite(LED_BUILTIN, LOW);
		Serial.print("failed, rc=");
		Serial.print(client.state());
		Serial.println(" try again in 2 seconds");
		digitalWrite(LED_BUILTIN, HIGH);
		delay(2000);
	}
}

// ticker calls this function to update brightness level of LED
void update_leds(void*)
{
	blue_led.fade();
}

void update_mqtt_status(void*)
{
	if(client.connected())
		client.publish("/home/flur/flurblume/status", String(blue_led.isOn()).c_str() );
}

// arduino framework calls this function to update mqtt client and ticker
void loop() {

	wifiMulti.run();

	if(WiFi.status() == WL_CONNECTED) {

		if(!client.connected())
		{
			reconnect();
		} 
		else 
		{
			client.loop();
		}
	}

	ticker.update();
}

