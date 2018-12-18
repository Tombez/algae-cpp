#include "./Socket.hpp"
// #if defined(unix) || defined(__unix) || defined(__unix__) || defined (__linux__)
// #elif defined (_WIN32)
	#include "./inet_pton4.hpp"
// #else
// 	#error "unsupported system (expected unix or windows)"
// #endif

#include <stdio.h>
#include <stdint.h>
#include <string.h>

void getsGood(char dest[], size_t len) {
	fgets(dest, len, stdin);
	dest[strcspn(dest, "\n")] = '\0';
}

int strToUint(char *text) {
	uint32_t res = 0;
	uint8_t i = 0;
	uint32_t mult = 1;
	uint8_t c = text[i];
	while (c != '\0') {
		c -= 48;
		if (c < 10) res += c * mult;
		else return res;
		mult *= 10;
		++i;
	}
	return res;
}

sock::Socket socky(1400);
struct sockaddr_in dest;

void onReceive(struct sockaddr_in *from, uint8_t *data, uint16_t dataLen) {
	printf("%s:%d> %s\n", inet_ntoa(from->sin_addr), ntohs(from->sin_port), data);
	if (strcmp((char*)data, "Ping?") == 0) {
		socky.sendMessage(from, (uint8_t*)"Pong!\0", 6);
	}
	if (strcmp((char*)data, "Pong!") == 0) {
		socky.sendMessage(from, (uint8_t*)"Nice. B)\0", 9);
	}
	if (strcmp((char*)data, "Nice. B)") == 0) {
		socky.sendMessage(from, (uint8_t*)"Very nice. ;)\0", 14);
	}
	if (strcmp((char*)data, "Very nice. ;)") == 0) {
		printf("Done!");
		exit(EXIT_SUCCESS);
	}
}

int main() {
	memset((char*)&dest, 0, sizeof(dest));
	socky.listen(49152, onReceive);

	char portStr[5 + 500 + 1];
	unsigned int port;

	char destIP[256];
	printf("IP address to connect to: ");
	getsGood(destIP, sizeof(destIP));
	int code = inet_pton4(destIP, (u_char*)&dest.sin_addr);
	if (code == 0) {
		printf("Invalid IP address!\n");
		exit(EXIT_FAILURE);
	}

	// printf("Destination port: ");
	// getsGood(portStr, sizeof(portStr));
	// port = strToUint(portStr);
	// if (port == 0) {
	// 	printf("Invalid port number!\n");
	// 	exit(EXIT_FAILURE);
	// }
	// printf("port: %i\n", port);

	//setup address structure
	dest.sin_family = AF_INET;
	dest.sin_port = htons((unsigned short)49152);

	socky.sendMessage(&dest, (uint8_t*)"Ping?\0", 6);

	while(1) {
		socky.processMessages();
	}

	return 0;
}
