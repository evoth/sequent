#include "cameraPTPIP.h"
#include <elapsedMillis.h>

// TODO: Reduce repeated structs and code

bool CameraPTPIP::readResponse(WiFiClient& client, char* buffer, size_t size) {
  elapsedMillis elapsed;
  while (client.available() == 0) {
    if (elapsed > 5000) {
      logger.error("Client timeout...");
      // cameraConnected = false;
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
    uint32_t transactionId;
    uint32_t param = 0x01;
  } operationRequest;
  using OperationRequestBuffer = char[22];

  auto operationRequestBuffer =
      reinterpret_cast<OperationRequestBuffer*>(&operationRequest);

  operationRequest.operation = 0x1002;
  operationRequest.transactionId = getTransactionId();
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
    logger.error(operationResponse.response,
                 "Bad Operation Response Code (0x%04x) for transaction ID %d",
                 operationResponse.response, operationResponse.transactionId);
    return;
  }

  // Unknown purpose (the elusive 0x902f operation, as seen in Wireshark)

  struct Operation902F {
    uint32_t length = 18;
    uint32_t packetType = 0x06;
    uint32_t dataPhase = 0x01;
    uint16_t operation = 0x902f;
    uint32_t transactionId;
  } operation902F;
  using Operation902FBuffer = char[18];

  auto operation902FBuffer =
      reinterpret_cast<Operation902FBuffer*>(&operation902F);

  operation902F.transactionId = getTransactionId();
  commandClient.write(*operation902FBuffer, sizeof(*operation902FBuffer));

  if (!readResponse(commandClient, *operationResponseBuffer,
                    sizeof(*operationResponseBuffer)))
    return;

  if (operationResponse.response != 0x2001) {
    logger.error(operationResponse.response,
                 "Bad Operation Response Code (0x%04x) for transaction ID %d",
                 operationResponse.response, operationResponse.transactionId);
    return;
  }

  // 2. Activate RemoteMode

  operationRequest.operation = 0x9114;
  operationRequest.transactionId = getTransactionId();
  commandClient.write(*operationRequestBuffer, sizeof(*operationRequestBuffer));

  if (!readResponse(commandClient, *operationResponseBuffer,
                    sizeof(*operationResponseBuffer)))
    return;

  if (operationResponse.response != 0x2001) {
    logger.error(operationResponse.response,
                 "Bad Operation Response Code (0x%04x) for transaction ID %d",
                 operationResponse.response, operationResponse.transactionId);
    return;
  }

  // 3. Activate EventMode

  operationRequest.operation = 0x9115;
  operationRequest.transactionId = getTransactionId();
  commandClient.write(*operationRequestBuffer, sizeof(*operationRequestBuffer));

  if (!readResponse(commandClient, *operationResponseBuffer,
                    sizeof(*operationResponseBuffer)))
    return;

  if (operationResponse.response != 0x2001) {
    logger.error(operationResponse.response,
                 "Bad Operation Response Code (0x%04x) for transaction ID %d",
                 operationResponse.response, operationResponse.transactionId);
    return;
  }

  pollEvents();

  logger.log("Connected to camera at %s.", cameraIP);
  cameraConnected = true;
#pragma pack(pop)
}

// https://julianschroden.com/post/2023-05-28-controlling-properties-using-ptp-ip-on-canon-eos-cameras/#geteventdata-operation
void CameraPTPIP::pollEvents() {
  struct GetEventDataRequest {
    uint32_t length = 18;
    uint32_t packetType = 0x06;
    uint32_t dataPhase = 0x01;
    uint16_t operation = 0x9116;
    uint32_t transactionId;
  } getEventDataRequest;
  using GetEventDataRequestBuffer = char[18];

  auto getEventDataRequestBuffer =
      reinterpret_cast<GetEventDataRequestBuffer*>(&getEventDataRequest);

  getEventDataRequest.transactionId = getTransactionId();
  commandClient.write(*getEventDataRequestBuffer,
                      sizeof(*getEventDataRequestBuffer));

  // Discard event data
  readResponse(commandClient, NULL, 0);
}

// Adapted from
// https://julianschroden.com/post/2023-06-15-capturing-images-using-ptp-ip-on-canon-eos-cameras/#image-capture-sequence
// https://github.com/laheller/ptplibrary/blob/master/library/src/main/java/com/rupiapps/ptp/constants/PtpOc.java
// From looking at Wireshark, it seems like my camera uses a capture phase of
// 0x03 instead of 0x01 and 0x02
void CameraPTPIP::triggerShutter() {
#pragma pack(push, 1)
  // Get rid of any unexpected data
  while (commandClient.available())
    commandClient.read();

  // Start Remote Release

  struct StartImageCapture {
    uint32_t length = 26;
    uint32_t packetType = 0x06;
    uint32_t dataPhase = 0x01;
    uint16_t operation = 0x9128;
    uint32_t transactionId;
    uint32_t capturePhase = 0x03;
    uint32_t unknownParameter = 0x0;
  } startImageCapture;
  using StartImageCaptureBuffer = char[26];

  auto startImageCaptureBuffer =
      reinterpret_cast<StartImageCaptureBuffer*>(&startImageCapture);

  startImageCapture.transactionId = getTransactionId();
  commandClient.write(*startImageCaptureBuffer,
                      sizeof(*startImageCaptureBuffer));

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
    logger.error(operationResponse.response,
                 "Bad Operation Response Code (0x%04x) for transaction ID %d",
                 operationResponse.response, operationResponse.transactionId);
    return;
  }

  // Unknown purpose (the elusive 0x902f operation, as seen in Wireshark)

  struct Operation902F {
    uint32_t length = 18;
    uint32_t packetType = 0x06;
    uint32_t dataPhase = 0x01;
    uint16_t operation = 0x902f;
    uint32_t transactionId;
  } operation902F;
  using Operation902FBuffer = char[18];

  auto operation902FBuffer =
      reinterpret_cast<Operation902FBuffer*>(&operation902F);

  operation902F.transactionId = getTransactionId();
  commandClient.write(*operation902FBuffer, sizeof(*operation902FBuffer));

  if (!readResponse(commandClient, *operationResponseBuffer,
                    sizeof(*operationResponseBuffer)))
    return;

  if (operationResponse.response != 0x2001) {
    logger.error(operationResponse.response,
                 "Bad Operation Response Code (0x%04x) for transaction ID %d",
                 operationResponse.response, operationResponse.transactionId);
    return;
  }

  // Stop Remote Release

  struct StopImageCapture {
    uint32_t length = 22;
    uint32_t packetType = 0x06;
    uint32_t dataPhase = 0x01;
    uint16_t operation = 0x9129;
    uint32_t transactionId;
    uint32_t capturePhase = 0x03;
  } stopImageCapture;
  using StopImageCaptureBuffer = char[22];

  auto stopImageCaptureBuffer =
      reinterpret_cast<StopImageCaptureBuffer*>(&stopImageCapture);

  stopImageCapture.transactionId = getTransactionId();
  commandClient.write(*stopImageCaptureBuffer, sizeof(*stopImageCaptureBuffer));

  if (!readResponse(commandClient, *operationResponseBuffer,
                    sizeof(*operationResponseBuffer)))
    return;

  if (operationResponse.response != 0x2001) {
    logger.error(operationResponse.response,
                 "Bad Operation Response Code (0x%04x) for transaction ID %d",
                 operationResponse.response, operationResponse.transactionId);
    return;
  }

  logger.log("Triggered shutter.");

#pragma pack(pop)
}

// https://julianschroden.com/post/2023-05-28-controlling-properties-using-ptp-ip-on-canon-eos-cameras/#set-property-value
bool CameraPTPIP::setPropertyValue(uint32_t propertyCode,
                                   uint32_t propertyValue) {
#pragma pack(push, 1)
  // Get rid of any unexpected data
  while (commandClient.available())
    commandClient.read();

  /* ===== SET PROPERTY VALUE ===== */

  // 1. Send Operation Request

  struct SetPropValue {
    uint32_t length = 18;
    uint32_t packetType = 0x06;
    uint32_t dataPhase = 0x02;
    uint16_t operation = 0x9110;
    uint32_t transactionId;
  } setPropValue;
  using SetPropValueBuffer = char[18];

  auto setPropValueBuffer =
      reinterpret_cast<SetPropValueBuffer*>(&setPropValue);

  setPropValue.transactionId = getTransactionId();
  commandClient.write(*setPropValueBuffer, sizeof(*setPropValueBuffer));

  // 2. Send Start Data Packet

  struct StartDataPacket {
    uint32_t length = 20;
    uint32_t packetType = 0x09;
    uint32_t transactionId;
    uint64_t totalDataLength = 12;
  } startDataPacket;
  using StartDataPacketBuffer = char[20];

  auto startDataPacketBuffer =
      reinterpret_cast<StartDataPacketBuffer*>(&startDataPacket);

  startDataPacket.transactionId = setPropValue.transactionId;
  commandClient.write(*startDataPacketBuffer, sizeof(*startDataPacketBuffer));

  // 3. Send End Data Packet

  struct EndDataPacket {
    uint32_t length = 24;
    uint32_t packetType = 0x0c;
    uint32_t transactionId;
    uint32_t payloadLength = 12;
    uint32_t propertyCode;
    uint32_t propertyValue;
  } endDataPacket;
  using EndDataPacketBuffer = char[24];

  auto endDataPacketBuffer =
      reinterpret_cast<EndDataPacketBuffer*>(&endDataPacket);

  endDataPacket.propertyCode = propertyCode;
  endDataPacket.propertyValue = propertyValue;
  endDataPacket.transactionId = setPropValue.transactionId;
  commandClient.write(*endDataPacketBuffer, sizeof(*endDataPacketBuffer));

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
    return false;

  pollEvents();

  if (operationResponse.response != 0x2001) {
    logger.error(operationResponse.response,
                 "Bad Operation Response Code (0x%04x) for transaction ID %d",
                 operationResponse.response, operationResponse.transactionId);
    return false;
  }

  return true;

#pragma pack(pop)
}