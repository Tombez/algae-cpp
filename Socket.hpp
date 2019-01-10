#pragma once

#include "./whichSystem.hpp"

#include <cstdlib>
#include <cstdio>
#include <cstdint>

#if defined(unix)
	#include <unistd.h>
	#include <errno.h>
	#include <netdb.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <netinet/in.h>
	// #define _GNU_SOURCE // See feature_test_macros(7)
	#include <signal.h>
	// #include <poll.h>
	#include <fcntl.h>
	#define sockerr errno
	#define SOCKET_ERROR -1
	//#define SOCKET_WOULD_BLOCK EWOULDBLOCK
#elif defined(_WIN32)
	#include <winsock2.h> // #pragma comment(lib,"ws2_32.lib")
	#define socklen_t int
	#define poll WSAPoll
	#define nfds_t ULONG
	#define sockerr WSAGetLastError()
	#define EWOULDBLOCK WSAEWOULDBLOCK
	#define close closesocket
#endif

namespace sock {
	bool initCalled = false;

	void init() {
		#if defined(_WIN32)
			//Initialise winsock
			WSADATA wsa;
			printf("Initialising Winsock...");
			if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
				printf("WSAStartup() failed with error code : %d", sockerr);
				exit(EXIT_FAILURE);
			}
			printf("Initialised.\n");
		#endif
	}
	void setNonBlocking(int socketID) {
		#if defined(unix)
			int flags = 0;
			flags = fcntl(socketID, F_GETFL, 0);
			if (flags == -1) flags = 0;
			fcntl(socketID, F_SETFL, flags | O_NONBLOCK);
		#elif defined(_WIN32)
			unsigned long on = 1;
			if (ioctlsocket(socketID, FIONBIO, &on) != 0) {
				printf("ioctlsocket() failed with error code : %d", sockerr);
				exit(EXIT_FAILURE);
			}
		#endif
	}
	typedef void (*MessageCallback)(struct sockaddr_in*, uint8_t*, uint16_t);
	class Socket {
	public:
		uint16_t bufferSize;
		uint8_t *buffer;
		int sockID;
		MessageCallback msgCb;
		struct sockaddr_in me;
		Socket(uint16_t bufferSize) : bufferSize(bufferSize), msgCb(NULL) {
			if (!initCalled) {
				init();
				initCalled = true;
			}
			buffer = new uint8_t[bufferSize];
			sockID = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			if (sockID == SOCKET_ERROR) {
				printf("socket() failed with error code : %d", sockerr);
				exit(EXIT_FAILURE);
			}
			printf("Socket created.\n");
			setNonBlocking(sockID);
		}
		void listen(uint16_t port, MessageCallback cb) {
			msgCb = cb;

			// memset(&me, 0, sizeof(me));
			me.sin_family = AF_INET;
			me.sin_addr.s_addr = INADDR_ANY;
			me.sin_port = htons(port);

			int code = bind(sockID, (struct sockaddr*)&me, sizeof(struct sockaddr_in));
			if (code == SOCKET_ERROR) {
				printf("bind() failed with error code : %d", sockerr);
				exit(EXIT_FAILURE);
			}
			puts("Bind done");
		}
		void processMessages() {
			struct sockaddr_in from;
			while(true) {
				socklen_t slen = sizeof(struct sockaddr_in);
				int len = recvfrom(sockID, (char*)buffer, bufferSize, 0, (struct sockaddr*)&from, &slen);
				if (len == SOCKET_ERROR) {
					if (sockerr == EWOULDBLOCK) {
						//printf("would have blocked...\n");
						break;
					}
					printf("recvfrom() failed with error code : %d", sockerr);
					exit(EXIT_FAILURE);
				}
				(*msgCb)(&from, buffer, uint16_t(len));
			}
		}
		void sendMessage(struct sockaddr_in *dest, uint8_t *message, uint16_t msgLength) {
			int code = sendto(sockID, (char*)message, msgLength, 0, (struct sockaddr*)dest, sizeof(struct sockaddr_in));
			if (code == SOCKET_ERROR) {
				printf("sendto() failed with error code : %d" , sockerr);
				exit(EXIT_FAILURE);
			}
		}
		~Socket() {
			close(sockID);
		}
	};
}
