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
 * ArpUtil.h
 *
 *  Created on: 16 Sep 2017
 *      Author: n1k0ism
 */



#ifndef ARPUTIL_H_
#define ARPUTIL_H_
#include "ArpPacket.h"

#define LED LED_BUILTIN

#define DEBUG

#define ETH_BUF_SIZE 400

class ArpUtil {

private:
	ArpPacket packet;

	static uint8_t L2_TYPE_ARP[2];// 0x0806
	static uint8_t ARP_HW_ETHER[2];// 0x0001
	static uint8_t ARP_PROTO_IPV4[2];// 0x0800

	bool connected = false;
	char deviceMac[18];
	uint8_t buffer[6];

	void initPacket();
	bool parseuint8_ts(const char* str, char delim, uint8_t* uint8_ts);
	void setMac(const char* mac, uint8_t* field);
	void setMac(const uint8_t* mac, uint8_t* field);
	void setIp(const char* ip, uint8_t* field);
	void setIp(const uint8_t* ip, uint8_t* field);

public:
	static uint8_t ARP_REQUEST[2];// 0x0001
	static uint8_t ARP_REPLY[2];// 0x0002

	uint8_t* gwIp;
	uint8_t* myIp;
	uint8_t* netmask;
	uint8_t* dnsIp;

	/*!
	 *  Prepares the ARP utility
	 *  \param mac the mac address ("a1:b2:c3:d4:e5:f6") you'd like to have for your device
	 */
	void begin(const char* mac);

	/*!
	 *   Initialises the Ethernet port and tries to obtain
	 *   - ip address
	 *   - netmask
	 *   - gateway
	 *   - dns
	 *   \return true on success, false else
	 */
	bool connect();
	/*!
	 *   The connection state
	 *   \return connection state
	 */
	bool isConnected();

	/*!
	 *   Sends one ARP data packet.
	 */
	void send();

	/*!
	 *   Sets the L2 header destination mac
	 *   \param mac the mac address (e.g. "a1:b2:c3:d4:e5:f6")
	 */
	void set_L2_DstMac(const char* mac);

	/*!
	 *   Sets the L2 header source mac
	 *   \param mac the mac address (e.g. "a1:b2:c3:d4:e5:f6")
	 */
	void set_L2_SrcMac(const char* mac);

	/*!
	 *   Sets the ARP destination mac
	 *   \param mac the mac address (e.g. "a1:b2:c3:d4:e5:f6")
	 */
	void set_ARP_DstMac(const char* mac);

	/*!
	 *   Sets the ARP source mac
	 *   \param mac the mac address (e.g. "a1:b2:c3:d4:e5:f6")
	 */
	void set_ARP_SrcMac(const char* mac);

	/*!
	 *   Sets the L2 header AND ARP destination mac. This is in
	 *   the most cases more convenient.
	 *   \param mac the mac address (e.g. "a1:b2:c3:d4:e5:f6")
	 */
	void set_DstMac(const char* mac);

	/*!
	 *   Sets the L2 header AND ARP source mac. This is in
	 *   the most cases more convenient.
	 *   \param mac the mac address (e.g. "a1:b2:c3:d4:e5:f6")
	 */
	void set_SrcMac(const char* mac);

	/*!
	 *   Sets the ARP operation code
	 *   \param opcode ARP_REQUEST, ARP_REPLY
	 */
	void set_OpCode(const uint8_t* opcode);

	/*!
	 *   Sets the destination ip address
	 *   \param ip the ip address (e.g. "192.168.0.123")
	 */
	void set_DstIp(const char* ip);

	/*!
	 *   Sets the destination ip address
	 *   \param ip the ip address as uint8_t array
	 */
	void set_DstIp(const uint8_t* ip);

	/*!
	 *   Sets the source ip address
	 *   \param ip the ip address (e.g. "192.168.0.123")
	 */
	void set_SrcIp(const char* ip);

	/*!
	 *   Sets the source ip address
	 *   \param ip the ip address as uint8_t array
	 */
	void set_SrcIp(const uint8_t* ip);
};

extern ArpUtil arputil;

#endif /* ARPUTIL_H_ */
