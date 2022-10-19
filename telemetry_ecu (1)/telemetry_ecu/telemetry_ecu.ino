#include "mcp_can.h"
#include "LoRa.h"
#include <SPI.h>
#include "telemetry_ecu.h"
 

MCP_CAN CAN(CAN_CS); // CAN Telemetría

INT8U ext = 0; // CAN_EXTID：Extended data frame, based on CAN 2.0B standard. ID range: 0 ~ 0x1FFFFFFF
INT8U len; // Asignar el número de bytes (8 bits) que se transmiten.

INT8U rxBuf[8] = {0,0,0,0,0,0,0,0}; // Único buffer para recibir
INT8U txBuf[8] = {0,0,0,0,0,0,0,0}; // Buffer para transmitir por el CAN

INT32U rxID; // ID para recibir (único para todas las recepciones)

int recibiendo = 0;
int start, end;
int tiempo;

//INT8U loraBuf[N_LORA_BUF]; // Buffer del LoRa

void setup()
{
  Serial.begin(115200);
  
  // Config de módulo LoRa
  LoRa.setSPIFrequency(10E6);
  //Init LoRa
  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  } else {
    Serial.println("LoRa: inicializacion correcta");
  }

  LoRa.setSignalBandwidth(500E3); //ancho de banda - entre 125 kHz - 500 kHz --> depende del rango de la señal
  LoRa.setSyncWord(0xFF); //crea una red cerrada solo para el emisor y el receptor
  LoRa.setSpreadingFactor(8); //ranges from 6-12,default 7 see API docs
  LoRa.setCodingRate4(7);

  //SPI.setClockDivider(SPI_CLOCK_DIV2); // Set SPI to run at 8MHz (16MHz / 2 = 8 MHz)


// Configuración de pines
  pinMode(CAN_INT,INPUT);

  // Inicialización del CAN de Telemetría
  if (CAN.begin(MCP_ANY,CAN_KBPS,MCP_8MHZ) == CAN_OK) {
    Serial.println("CAN: Inicializacion correcta!");
    CAN.setMode(MCP_NORMAL);
  } else {
    Serial.println("CAN: Fallo al incializar, reinicie por favor");
    exit(0);
  }
  
  // ---------- SECUENCIA DE ARRANQUE ----------
  // Config de sensores

  // Config de módulo GPS

  // Esperamos a que el Master ECU nos pregunte si estamos listos
  while (ecu_master == 0) {
    if (!digitalRead(CAN_INT)) {
      CAN.readMsgBuf(&rxID, &len, rxBuf);
      if (rxID == ID_ok_telemetria && len == 1 && rxBuf[0] == 1) {
        len = 1;
        txBuf[0] = 1;
        CAN.sendMsgBuf(ID_ack_telemetria, ext, len, txBuf);
        ecu_master = 1; // Telemetría arrancada
        Serial.println("Telemetria arrancada correctamente.");
      }
    }
  }
  
  // Esperamos hasta que el Master ECU nos mande la señal de READY-TO-DRIVE
  while (RTD_all == 0) {
    if (!digitalRead(CAN_INT)) {
      CAN.readMsgBuf(&rxID, &len, rxBuf);
      if (rxID == ID_RTD_all && len == 1 && rxBuf[0] == 1) {
        RTD_all = 1; // Telemetría lista
        Serial.println("Telemetria RTD, enviando datos.");
      }
    }
  }
}

void loop() {
  if(!digitalRead(CAN_INT)) {
    // Leemos del CAN
    Serial.println("Recibiendo...");
    if(CAN.readMsgBuf(&rxID, &len, rxBuf) == CAN_OK) {
      Serial.println(rxID,HEX);
      if(rxID == 0x12D){
        Serial.println("Recibido baterias");
      }
      sendMsg(rxID,len,rxBuf);
      Serial.println(rxBuf[1]);
    }
  }
}

void sendMsg(INT32U rxId, int len, INT8U *rxBuf) {
  Serial.println(millis());
  LoRa.beginPacket();                   // start packet
  LoRa.print("0x");
  LoRa.print(rxId,HEX);                 // add payload
  LoRa.print(",");
  LoRa.print(len);
  LoRa.print(",");
  LoRa.print(rxBuf[0]);
  LoRa.print(",");
  LoRa.println(rxBuf[1]);
  LoRa.endPacket();                     // finish packet and send it
  Serial.println(millis());
}
