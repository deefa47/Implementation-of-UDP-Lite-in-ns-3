#ifndef UDPLITE_HEADER_H
#define UDPLITE_HEADER_H

#include <stdint.h>
#include <string.h>

#include "udp-header.h"
#include "ns3/header.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"

namespace ns3{

	class UdpliteHeader : public UdpHeader
	{	
		public :
		UdpliteHeader();
		~UdpliteHeader();
		void SetChecksumCoverage (uint16_t cscov);
		uint16_t GetChecksumCoverage ();
		void SetTestPass (bool value);
		bool GetTestPass ();
	  void SetLimit (uint16_t limit);
		uint16_t GetLimit ();
		//GetChecksum,forcepayloadsize, forcechecksum, ischecksumok,print,getSerialized size, initializechecksum(3), ischecksumok
		static TypeId GetTypeId(void);
		virtual TypeId GetInstanceTypeId (void) const;
		virtual void Print (std::ostream &os) const;
    virtual uint32_t GetSerializedSize (void) const;
		virtual void Serialize (Buffer::Iterator start) const;
		virtual uint32_t Deserialize (Buffer::Iterator start);
		bool IsLimitSet (void) const;
		private :
		uint16_t CalculateHeaderChecksum (uint16_t size) const;
		uint16_t m_checksumCoverage;
		uint16_t m_limit;
		bool m_testPass;
	};
}
#endif