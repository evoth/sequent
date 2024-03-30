#include "cameraPTPIP.h"
#include <elapsedMillis.h>

bool CameraPTPIP::readResponse(WiFiClient& client, char* buffer, size_t size) {
  elapsedMillis elapsed;
  while (client.available() == 0) {
    if (elapsed > 5000) {
      logger.error("Client timeout...");
      client.stop();
      return false;
    }
  }

  int i = 0;
  while (client.available()) {
    if (i < size) {
      buffer[i] = client.read();
      i++;
    } else {
      client.read();
    }
  }

  return true;
}

// https://julianschroden.com/post/2023-05-10-pairing-and-initializing-a-ptp-ip-connection-with-a-canon-eos-camera/#connection-sequence
void CameraPTPIP::connect() {
#pragma pack(push, 1)
  /* ===== CONNECTION SEQUENCE ===== */

  // 1. Open Command Socket Connection
  if (!commandClient.connect(cameraIP, 15740)) {
    logger.error("Command client could not connect to %s at port 15740.",
                 cameraIP);
    return;
  }

  // 2. Send Init Command Request
  struct InitCommandRequest {
    uint32_t length = 40;
    uint32_t packetType = 0x01;
    uint8_t clientGuid[16] = {0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77,
                              0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77};
    char16_t clientName[6] = u"ESP32";
    uint32_t ptpVersion = 0x010000;
  } initCommandRequest;
  using InitCommandRequestBuffer = char[40];

  auto initCommandRequestBuffer =
      reinterpret_cast<InitCommandRequestBuffer*>(&initCommandRequest);

  commandClient.write(*initCommandRequestBuffer,
                      sizeof(*initCommandRequestBuffer));

  // 3. Process Init Command Acknowledgement Response
  struct InitCommandAck {
    uint32_t length;
    uint32_t packetType;
    uint32_t connectionNumber;
  } initCommandAck;
  using InitCommandAckBuffer = char[12];

  auto initCommandAckBuffer =
      reinterpret_cast<InitCommandAckBuffer*>(&initCommandAck);

  if (!readResponse(commandClient, *initCommandAckBuffer,
                    sizeof(*initCommandAckBuffer)))
    return;

  if (initCommandAck.packetType != 0x02) {
    logger.error("Bad Init Command Ack (%02x)", initCommandAck.packetType);
    return;
  }

  // 4. Open Event Socket Connection
  if (!eventClient.connect(cameraIP, 15740)) {
    logger.error("Event client could not connect to %s at port 15740.",
                 cameraIP);
    return;
  }

  // 5. Send Init Event Request
  struct InitEventRequest {
    uint32_t length = 12;
    uint32_t packetType = 0x03;
    uint32_t connectionNumber;
  } initEventRequest;
  using InitEventRequestBuffer = char[12];

  auto initEventRequestBuffer =
      reinterpret_cast<InitEventRequestBuffer*>(&initEventRequest);

  initEventRequest.connectionNumber = initCommandAck.connectionNumber;
  eventClient.write(*initEventRequestBuffer, sizeof(*initEventRequestBuffer));

  // 6. Process Init Event Acknowledgement Response
  struct InitEventAck {
    uint32_t length;
    uint32_t packetType;
  } initEventAck;
  using InitEventAckBuffer = char[8];

  auto initEventAckBuffer =
      reinterpret_cast<InitEventAckBuffer*>(&initEventAck);

  if (!readResponse(eventClient, *initEventAckBuffer,
                    sizeof(*initEventAckBuffer)))
    return;

  if (initEventAck.packetType != 0x04) {
    logger.error("Bad Init Event Ack (%02x)", initEventAck.packetType);
    return;
  }

  /* ===== INITIALIZING THE CONNECTION ===== */

  // 1. OpenSession

  struct OperationRequest {
    uint32_t length = 22;
    uint32_t packetType = 0x06;
    uint32_t dataPhase = 0x01;
    uint16_t operation;
    uint32_t transactionId = 0;
    uint32_t param = 0x01;
  } operationRequest;
  using OperationRequestBuffer = char[22];

  auto operationRequestBuffer =
      reinterpret_cast<OperationRequestBuffer*>(&operationRequest);

  operationRequest.operation = 0x1002;
  commandClient.write(*operationRequestBuffer, sizeof(*operationRequestBuffer));

  struct OperationResponse {
    uint32_t length;
    uint32_t packetType;
    uint16_t response;
    uint32_t transactionId;
  } operationResponse;
  using OperationResponseBuffer = char[14];

  auto operationResponseBuffer =
      reinterpret_cast<OperationResponseBuffer*>(&operationResponse);

  if (!readResponse(commandClient, *operationResponseBuffer,
                    sizeof(*operationResponseBuffer)))
    return;

  if (operationResponse.response != 0x2001) {
    logger.error("Bad Operation Response Code (%04x) for transaction ID %d",
                 operationResponse.response, operationResponse.transactionId);
    return;
  }

  // 2. Activate RemoteMode

  operationRequest.operation = 0x9114;
  operationRequest.transactionId++;
  commandClient.write(*operationRequestBuffer, sizeof(*operationRequestBuffer));

  if (!readResponse(commandClient, *operationResponseBuffer,
                    sizeof(*operationResponseBuffer)))
    return;

  if (operationResponse.response != 0x2001) {
    logger.error("Bad Operation Response Code (%04x) for transaction ID %d",
                 operationResponse.response, operationResponse.transactionId);
    return;
  }

  // 3. Activate EventMode

  operationRequest.operation = 0x9115;
  operationRequest.transactionId++;
  commandClient.write(*operationRequestBuffer, sizeof(*operationRequestBuffer));

  if (!readResponse(commandClient, *operationResponseBuffer,
                    sizeof(*operationResponseBuffer)))
    return;

  if (operationResponse.response != 0x2001) {
    logger.error("Bad Operation Response Code (%04x) for transaction ID %d",
                 operationResponse.response, operationResponse.transactionId);
    return;
  }

  logger.log("Connected to camera at %s.", cameraIP);
  cameraConnected = true;
#pragma pack(pop)
}