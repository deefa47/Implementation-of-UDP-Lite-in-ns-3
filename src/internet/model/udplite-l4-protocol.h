#ifndef UDPLITE_L4_PROTOCOL_H
#define UDPLITE_L4_PROTOCOL_H

#include <stdint.h>
#include "ns3/packet.h"
#include "ns3/ptr.h"
#include "ip-l4-protocol.h"
#include "udp-l4-protocol.h"

namespace ns3{
class Node;
class Socket;
class Ipv4EndPointDemux;
class Ipv4EndPoint;
class Ipv6EndPointDemux;
class Ipv6EndPoint;
class UdpSocketImpl;
	class UdpliteL4Protocol : public UdpL4Protocol{
	public: 
		static TypeId GetTypeId (void);
		static const uint8_t PROT_NUMBER;
		Ptr<Socket> CreateSocket (void);
			void NotifyNewAggregate ();

		void Send (Ptr<Packet> packet,
            Ipv4Address saddr, Ipv4Address daddr, 
            uint16_t sport, uint16_t dport, uint16_t checksumCoverage);
		void Send (Ptr<Packet> packet,
             Ipv4Address saddr, Ipv4Address daddr, 
             uint16_t sport, uint16_t dport, Ptr<Ipv4Route> route, uint16_t checksumCoverage);
		void Send (Ptr<Packet> packet,
             Ipv6Address saddr, Ipv6Address daddr, 
             uint16_t sport, uint16_t dport, uint16_t checksumCoverage);
		void Send (Ptr<Packet> packet,
             Ipv6Address saddr, Ipv6Address daddr, 
             uint16_t sport, uint16_t dport, Ptr<Ipv6Route> route, uint16_t checksumCoverage);

	protected:
		void DoDispose (void);
	};
}
#endif