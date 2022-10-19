#include <Wire.h>
#include <SPI.h>
#include "LoRa.h"

byte buf[2] = {0, 0};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  //SPI.begin();
  
  //Init LoRa
  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  } else {
    Serial.println("LoRa initialized succesfully");
  }
  LoRa.setSignalBandwidth(500E3); //ancho de banda - entre 125 kHz - 500 kHz --> depende del rango de la señal
  LoRa.setSyncWord(0xFF); //crea una red cerrada solo para el emisor y el receptor
  LoRa.setSpreadingFactor(8); //ranges from 6-12,default 7 see API docs
  LoRa.setCodingRate4(7);
}

void loop() {

  receiveMsg(LoRa.parsePacket());

}


void receiveMsg(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

  // read packet header bytes:
  String incoming = "";
  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }
    Serial.print(incoming);
    //Serial.println("RSSI: " + String(LoRa.packetRssi()) + "dBm");  //Recieved signal stength
    //Serial.println("Snr: " + String(LoRa.packetSnr()) + "dBm");    //Signal to node ratio 
    //Serial.println();
}
