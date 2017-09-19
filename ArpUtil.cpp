/*
 * Copyright 2017 Nikolai M. Langer
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * ArpUtil.cpp
 *
 *  Created on: 16 Sep 2017
 *      Author: n1k0ism
 */
#include "ArpUtil.h"

#include <Arduino.h>
#include <enc28j60.h>
#include <EtherCard.h>
#include <net.h>
#include "ArpPacket.h"

ArpUtil arputil;

uint8_t ArpUtil::L2_TYPE_ARP[SIZE_WORD] = { 0x08, 0x06 };
uint8_t ArpUtil::ARP_HW_ETHER[SIZE_WORD] = { 0x00, 0x01 };
uint8_t ArpUtil::ARP_PROTO_IPV4[SIZE_WORD] = { 0x08, 0x00 };
uint8_t ArpUtil::ARP_REQUEST[SIZE_WORD] = { 0x00, 0x01 };
uint8_t ArpUtil::ARP_REPLY[SIZE_WORD] = { 0x00, 0x02 };

uint8_t Ethernet::buffer[SIZE_ETH_BUF];

void ArpUtil::initPacket() {
    // these initial values can be changed any time by using
    // the provided functions
    set_DstMac("ff:ff:ff:ff:ff:ff");
    set_SrcMac(deviceMac);
    set_DstIp("255.255.255.255");
    set_SrcIp("192.168.0.1");
    set_OpCode(ARP_REQUEST);

    // hardware and protocol settings (do not change)
    memcpy(packet.data.l2.type, L2_TYPE_ARP, SIZE_WORD);
    memcpy(packet.data.arp.typeHw, ARP_HW_ETHER, SIZE_WORD);
    memcpy(packet.data.arp.typeProto, ARP_PROTO_IPV4, SIZE_WORD);
    packet.data.arp.sizeHw = SIZE_ETHER;
    packet.data.arp.sizeProto = SIZE_IPV4;
}

bool ArpUtil::parseuint8_ts(const char* str, char delim, uint8_t* uint8_ts) {
    // defaults for mac address
    int base = 16;
    int siz = SIZE_ETHER;

    // adapt if it is an ip address
    if (delim == '.') {
        base = 10;
        siz = SIZE_IPV4;
    }
    // iterate through delimited string
    for (int i = 0; i < siz; i++) {
        // convert to uint8_t
        uint8_ts[i] = strtoul(str, NULL, base);
        // get next
        str = strchr(str, delim);
        // finished?
        if (str == NULL || *str == '\0') {
            return ((i + 1) == siz);
        }
        str++;
    }
    return false;
}
void ArpUtil::setMac(const char* mac, uint8_t* field) {
    this->parseuint8_ts(mac, ':', buffer);
    setMac(buffer, field);
}

void ArpUtil::setIp(const char* ip, uint8_t* field) {
    this->parseuint8_ts(ip, '.', buffer);
    setIp(buffer, field);
}

void ArpUtil::setMac(const uint8_t* mac, uint8_t* field) {
    memcpy(field, mac, SIZE_ETHER);
}

void ArpUtil::setIp(const uint8_t* ip, uint8_t* field) {
    memcpy(field, ip, SIZE_IPV4);
}

void ArpUtil::begin(const char* devMac) {
    memcpy(deviceMac, devMac, SIZE_MAC_ASCII);
#ifdef DEBUG
    Serial.begin(115200);
    Serial.println(deviceMac);
#endif
    pinMode(LED, OUTPUT);
    initPacket();
}

bool ArpUtil::isConnected() {
    return connected;
}

void ArpUtil::send() {
    digitalWrite(LED, HIGH);
    for (unsigned int i = 0; i < sizeof(packet); i++) {
        ether.buffer[i] = this->packet.raw[i];
        ether.packetSend(sizeof(packet));
    }
    digitalWrite(LED, LOW);
}

bool ArpUtil::connect() {
#ifdef DEBUG
    Serial.print("Opening Ethernet port...");
#endif
    parseuint8_ts(deviceMac, ':', buffer);
    if (ether.begin(sizeof Ethernet::buffer, buffer) == 0) {
#ifdef DEBUG
        Serial.println("fail!");
#endif
        connected = false;
        return connected;
    }
#ifdef DEBUG
    Serial.println("success!");

    Serial.print("Waiting for dhcp assignment...");
#endif
    if (!ether.dhcpSetup()) {
#ifdef DEBUG
        Serial.println("fail!");
#endif
        connected = false;

    } else {
        ArpUtil::myIp = ether.myip;
        ArpUtil::gwIp = ether.gwip;
        ArpUtil::netmask = ether.netmask;
        ArpUtil::dnsIp = ether.dnsip;
        connected = true;
#ifdef DEBUG
        Serial.println("success!");
        Serial.print("ether   ");
        Serial.println(deviceMac);
        ether.printIp("inet    ", myIp);
        ether.printIp("netmask ", netmask);
        ether.printIp("gateway ", gwIp);
        ether.printIp("dns     ", dnsIp);
#endif
    }
    return connected;
}

void ArpUtil::set_L2_DstMac(const char* mac) {
    setMac(mac, packet.data.l2.dstMac);
}

void ArpUtil::set_L2_SrcMac(const char* mac) {
    setMac(mac, packet.data.l2.srcMac);
}

void ArpUtil::set_ARP_DstMac(const char* mac) {
    setMac(mac, packet.data.arp.dstMac);
}

void ArpUtil::set_ARP_SrcMac(const char* mac) {
    setMac(mac, packet.data.arp.srcMac);
}

void ArpUtil::set_DstMac(const char* mac) {
    set_L2_DstMac(mac);
    set_ARP_DstMac(mac);
}

void ArpUtil::set_SrcMac(const char* mac) {
    set_L2_SrcMac(mac);
    set_ARP_SrcMac(mac);
}

void ArpUtil::set_OpCode(const uint8_t* opcode) {
    memcpy(packet.data.arp.opcode, opcode, SIZE_WORD);
}

void ArpUtil::set_SrcIp(const char* ip) {
    setIp(ip, packet.data.arp.srcIp);
}

void ArpUtil::set_SrcIp(const uint8_t* ip) {
    setIp(ip, packet.data.arp.srcIp);
}

void ArpUtil::set_DstIp(const char* ip) {
    setIp(ip, packet.data.arp.dstIp);
}
void ArpUtil::set_DstIp(const uint8_t* ip) {
    setIp(ip, packet.data.arp.dstIp);
}
