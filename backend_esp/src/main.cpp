#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>

#define SSID "T6_AP"
#define PASSWORD "defgecd7"

#define SERVER_HOST_NAME "192.168.1.2"

#define TCP_PORT 15740

AsyncClient* commandClient = new AsyncClient;
AsyncClient* eventClient = new AsyncClient;
char connectionNumber;

static void openSessionResponse(void* arg, AsyncClient* client, void *data, size_t len) {
	Serial.printf("\n data received from %s \n", client->remoteIP().toString().c_str());
  char *packet = (char *)data;
  Serial.printf("Response: 0x%x%x.\n", packet[9], packet[8]);
  if (packet[8] != '\x01' || packet[9] != '\x20') return;

  Serial.println("Session opened");
}

static void openSessionRequest() {
	// send reply
  const char packet[] = "\x16\x00\x00\x00\x06\x00\x00\x00\x01\x00\x00\x00\x02\x10\x00\x00\x00\x00\x01\x00\x00\x00";
	if (commandClient->space() > sizeof(packet) && commandClient->canSend()) {
		// char message[32];
		// sprintf(message, "this is from %s", WiFi.localIP().toString().c_str());
    Serial.println("sending...");
		commandClient->write(packet, sizeof(packet));
	  commandClient->onData(&openSessionResponse, commandClient);
	}
}

static void initEventAck(void* arg, AsyncClient* client, void *data, size_t len) {
	Serial.printf("\n data received from %s \n", client->remoteIP().toString().c_str());
  char *packet = (char *)data;
  if (packet[4] != '\x04') return;

  // openSessionRequest();
}

static void initEventRequest(void* arg) {
	// send reply
  char packet[] = "\x0c\x00\x00\x00\x03\x00\x00\x00\x01\x00\x00\x00";
  packet[8] = connectionNumber;
	if (eventClient->space() > sizeof(packet) && eventClient->canSend()) {
		// char message[32];
		// sprintf(message, "this is from %s", WiFi.localIP().toString().c_str());
    Serial.println("sending...");
		eventClient->write(packet, sizeof(packet));
	  eventClient->onData(&initEventAck, eventClient);
	}
}

void onEventConnect(void* arg, AsyncClient* client) {
	Serial.printf("\n event client has been connected to %s on port %d \n", SERVER_HOST_NAME, TCP_PORT);
	initEventRequest(client);
}

static void initCommandAck(void* arg, AsyncClient* client, void *data, size_t len) {
	Serial.printf("\n data received from %s \n", client->remoteIP().toString().c_str());
  char *packet = (char *)data;
  if (packet[4] != '\x02') return;
  connectionNumber = packet[8];

	eventClient->onConnect(&onEventConnect, eventClient);
	eventClient->connect(SERVER_HOST_NAME, TCP_PORT);
}

static void initCommandRequest(void* arg) {
	// send reply
  const char packet[] = "\x28\x00\x00\x00\x01\x00\x00\x00\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x45\x00\x53\x00\x50\x00\x33\x00\x32\x00\x00\x00\x00\x00\x01\x00";
	if (commandClient->space() > sizeof(packet) && commandClient->canSend()) {
		// char message[32];
		// sprintf(message, "this is from %s", WiFi.localIP().toString().c_str());
    Serial.println("sending...");
		commandClient->write(packet, sizeof(packet));
	  commandClient->onData(&initCommandAck, commandClient);
	}
}

// /* event callbacks */
// static void handleData(void* arg, AsyncClient* client, void *data, size_t len) {
// 	Serial.printf("\n data received from %s \n", client->remoteIP().toString().c_str());
// 	Serial.write((uint8_t*)data, len);
// }

void onCommandConnect(void* arg, AsyncClient* client) {
	Serial.printf("\n command client has been connected to %s on port %d \n", SERVER_HOST_NAME, TCP_PORT);
	initCommandRequest(client);
}


void setup() {
	Serial.begin(115200);
	delay(20);

	// connects to access point
	WiFi.mode(WIFI_STA);
	WiFi.begin(SSID, PASSWORD);
	while (WiFi.status() != WL_CONNECTED) {
		Serial.print('.');
		delay(500);
	}

	commandClient->onConnect(&onCommandConnect, commandClient);
	commandClient->connect(SERVER_HOST_NAME, TCP_PORT);
}

void loop() {

}