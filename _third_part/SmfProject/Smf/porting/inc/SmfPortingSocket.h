#pragma once
#include <stdint.h>
#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 46
void closeSocket(int);
/* Linux����sockaddr_in��ͬһ��ͷ�ļ� */
/* Internet address. */
//struct in_addr
//{
//	uint32_t s_addr;
//};
/* Linux�¶�����usr/include/netinet/in.h�� */
/* Structure describing an Internet socket address. */
//struct sockaddr_in
//{
//	uint16_t sin_family;
//	uint16_t sin_port;			/* Port number. */
//	struct in_addr sin_addr;	/* Internet address. */
//
//	/* Pad to size of 'struct sockaddr'. */
//	unsigned char sin_zero[sizeof(struct sockaddr) -
//		sizeof(uint16_t) -
//		sizeof(uint16_t) -
//		sizeof(struct in_addr)];
//	/* ��ʵ����unsigned char sin_zero[8]; */
//};


/* Linux����sockaddr_in��ͬһ��ͷ�ļ� */
/* IPv6 address */
//struct in6_addr
//{
//	union
//	{
//		uint8_t __u6_addr8[16];
//		uint16_t __u6_addr16[8];
//		uint32_t __u6_addr32[4];
//	}__in6_u;
//#define s6_addr		__in6_u.__u6_addr8
//#ifdef __USE_MISC
//#define s6_addr16 	__in6_u.__u6_addr16
//#define s6_addr32 	__in6_u.__u6_addr32
//#endif
//};

/* Linux�¸�sockaddr_in��ͬһ��ͷ�ļ� */
/* Ditto, for IPv6. */
struct sockaddr_in6
{
	uint16_t sin6_family;
	uint16_t sin6_port;			/* Transport layer port # */
	uint32_t sin6_flowinfo;		/* IPv6 flow information */
	struct in6_addr sin6_addr;	/* IPv6 address */
	uint32_t sin6_scope_id;		/* IPv6 scope-id */
};

struct ipv6_mreq {
	struct in6_addr ipv6mr_multiaddr; /* IPv6 multicast addr */
	unsigned int ipv6mr_interface; /* interface index */
};
#endif