//------------------------------------------------------------------------------
//  TITLE :          uart_util.h
//  DESCRIPTION :    Processing PVCI Serial String.
//------------------------------------------------------------------------------

#include <iostream>
#include <string>

#include "project.h"


char* CalcChecksum(const char* sentence);

// Variables FROM motherboard
extern int lcd_config = 1;

extern uint8_t sta1 = 0b00000000;
extern uint8_t sta2 = 0b00000000;
extern uint8_t sta3 = 0b00000000;
extern uint8_t nfe = 0b00000000;
extern uint8_t sfe = 0b00000000;
extern uint8_t cfe = 0b00000000;
extern bool gal = false;

extern int portNozzlePosition = 0;
extern int stbdNozzlePosition = 0;
extern int portBucketPosition = 0;
extern int stbdBucketPosition = 0;
extern int portInterceptorPosition = 0;
extern int stbdInterceptorPosition = 0;

char pvci_arr[MAX_RAWINPUTSTRING_LENGTH][MAX_RAWINPUTSTRING_DIGITS];

// Variables TO SEND to motherboard
extern int intsteer = 0;
extern int set1_set2_mode = 0;
extern int set1_set2_flag = 0;
extern int position_capture_request = 0;

// Variables received from motherboard
extern int Autocal_Statusi = 0;

enum AutoCal_Command {
	NONE = 0,
	INITIALIZE = 11,
	ABORT = 33,
	FINISH = 22,
};

extern AutoCal_Command autocal_cmd = NONE;

void processSerial(void)
{
	char str[255] = { 0 };
	uint32_t screenPos;
	MessageQueueInfo_t queueInfo;

	GetQueueInfo(&queueInfo);

	uint8_t* buf = PeekMessage(PeekTail, 1, (uint8_t*)str, sizeof(str));
	std::string pvci_string(reinterpret_cast<char*>(buf));

	for (int i = 0; i < MAX_RAWINPUTSTRING_LENGTH; ++i) {
		for (int j = 0; j < MAX_RAWINPUTSTRING_DIGITS; ++j) {
			pvci_arr[i][j] = '\0';
		}
	}

	int segmentIndex = 0;
	int currentIndex = 0;

	for (char ch : pvci_string) {
		if (ch == ',') {
			pvci_arr[segmentIndex][currentIndex] = '\0';
			segmentIndex++;
			currentIndex = 0;
			continue;
		}
		if (currentIndex < MAX_RAWINPUTSTRING_DIGITS - 1) {
			pvci_arr[segmentIndex][currentIndex] = ch;
			currentIndex++;
		}
	}

	if (segmentIndex < MAX_RAWINPUTSTRING_LENGTH) {
		pvci_arr[segmentIndex][currentIndex] = '\0';
	}

	const char* ccptr = pvci_string.c_str();

	char sixthchar = ccptr[6];

	int mapped_portNozzlePosition = ((std::atoi(pvci_arr[1]) * 1000 / 999) - 500) / 50;
	int mapped_stbdNozzlePosition = ((std::atoi(pvci_arr[2]) * 1000 / 999) - 500) / 50;
	int mapped_portBucketPosition = ((std::atoi(pvci_arr[3]) * 1000 / 999) - 500) / 50;
	int mapped_stbdBucketPosition = ((std::atoi(pvci_arr[4]) * 1000 / 999) - 500) / 50;
	int mapped_portInterceptorPosition = (std::atoi(pvci_arr[5]) * 1000 / 999) / 50;
	int mapped_stbdInterceptorPosition = (std::atoi(pvci_arr[6]) * 1000 / 999) / 50;

	portBucketPosition = mapped_portBucketPosition;
	stbdBucketPosition = mapped_stbdBucketPosition;
	portNozzlePosition = mapped_portNozzlePosition;
	stbdNozzlePosition = mapped_stbdNozzlePosition;
	portInterceptorPosition = mapped_portInterceptorPosition;
	stbdInterceptorPosition = mapped_stbdInterceptorPosition;

	sfe = static_cast<uint8_t>(std::atoi(pvci_arr[7]));
	nfe = static_cast<uint8_t>(std::atoi(pvci_arr[8]));
	sta1 = static_cast<uint8_t>(std::atoi(pvci_arr[9]));
	sta2 = static_cast<uint8_t>(std::atoi(pvci_arr[10]));
	sta3 = static_cast<uint8_t>(std::atoi(pvci_arr[11]));
	cfe = static_cast<uint8_t>(std::atoi(pvci_arr[12]));

	if (!m_SYSTEMOPTIONS_DB[7].status)

		lcd_config = std::atoi(pvci_arr[14]);

	if (sfe | nfe | sta1 | sta2 | sta3) {
		gal = true;
	}

	Autocal_Statusi = std::atoi(pvci_arr[17]);
}

void sendSerial(void) {


	char messageWithoutChecksum[50];

	//snprintf(messageWithoutChecksum, sizeof(messageWithoutChecksum), "$PVCC,%d,%d,%d,%d,%d,%d,%d,%d", value0, value1, value2, value3, value4, value5, value6, value7);
	snprintf(messageWithoutChecksum, sizeof(messageWithoutChecksum), "$PVCC,%d,%d,%d,%d,%d,%d,%d,%d", intsteer, autocal_cmd, set1_set2_mode, set1_set2_flag, 0, 0, 0, 0);

	char* checksum = CalcChecksum(messageWithoutChecksum);
	char fullMessage[60]; //guestimate I counted 38bytes , but there could be more , so 60 is safe  
	snprintf(fullMessage, sizeof(fullMessage), "%s*%s\r", messageWithoutChecksum, checksum);
	uint32_t dataLen = strlen(fullMessage);
	UARTSend((uint8_t*)fullMessage, dataLen);
}
char* CalcChecksum(const char* msg) {
	static char checksumStr[3];
	int checksum = 0;
	if (msg[0] == '$') {
		msg++;
	}
	//while ( *msg != '*') {
	//	checksum ^= (unsigned char)(*msg++);
	//}
	while (*msg && *msg != '*') {
		checksum ^= (unsigned char)(*msg++);
	}
	snprintf(checksumStr, sizeof(checksumStr), "%02X", checksum);
	return checksumStr;
}
