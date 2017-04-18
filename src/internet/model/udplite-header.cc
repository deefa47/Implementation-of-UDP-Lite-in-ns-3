
#include "udplite-header.h"
#include "ns3/address-utils.h"

namespace ns3{

NS_OBJECT_ENSURE_REGISTERED (UdpliteHeader);


UdpliteHeader::UdpliteHeader ()
  : m_checksumCoverage (0),
    m_limit (0),
    m_testPass (true)
{	
}

UdpliteHeader::~UdpliteHeader ()
{
  m_checksumCoverage = 0xfffe;
  m_limit = 0xfffe;
}
void 
UdpliteHeader::Print (std::ostream &os) const
{
  os << "length: " << m_payloadSize + GetSerializedSize ()
     << " " 
     << m_sourcePort << " > " << m_destinationPort
  ;
}

uint32_t 
UdpliteHeader::GetSerializedSize (void) const
{
  return 8;
}

TypeId 
UdpliteHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::UdpliteHeader")
    .SetParent<Header> ()
    .SetGroupName ("Internet")
    .AddConstructor<UdpliteHeader> ()
  ;
  return tid;
}
TypeId 
UdpliteHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}
void
UdpliteHeader::SetChecksumCoverage (uint16_t cscov)
{
	m_checksumCoverage = cscov;
}
void
UdpliteHeader::SetTestPass(bool value)
{
  m_testPass=value;
}
bool
UdpliteHeader::GetTestPass ()
{
  return m_testPass;
}
uint16_t
UdpliteHeader::GetChecksumCoverage ()
{
	return m_checksumCoverage;
}
void
UdpliteHeader::SetLimit (uint16_t limit)
{
  m_limit = limit;
}

uint16_t
UdpliteHeader::GetLimit ()
{
  return m_limit;
}
bool
UdpliteHeader::IsLimitSet (void) const
{
  return (m_limit>0); 
}
uint16_t
UdpliteHeader::CalculateHeaderChecksum (uint16_t size) const
{
  Buffer buf = Buffer ((2 * Address::MAX_SIZE) + 8);
  buf.AddAtStart ((2 * Address::MAX_SIZE) + 8);
  Buffer::Iterator it = buf.Begin ();
  uint32_t hdrSize = 0;

  WriteTo (it, m_source);
  WriteTo (it, m_destination);
  if (Ipv4Address::IsMatchingType (m_source))
    {
      it.WriteU8 (0); /* protocol */
      it.WriteU8 (m_protocol); /* protocol */
      it.WriteU8 (size >> 8); /* length */
      it.WriteU8 (size & 0xff); /* length */
      hdrSize = 12;
    }
  else if (Ipv6Address::IsMatchingType (m_source))
    {
      it.WriteU16 (0);
      it.WriteU8 (size >> 8); /* length */
      it.WriteU8 (size & 0xff); /* length */
      it.WriteU16 (0);
      it.WriteU8 (0);
      it.WriteU8 (m_protocol); /* protocol */
      hdrSize = 40;
    }

  it = buf.Begin ();
  /* we don't CompleteChecksum ( ~ ) now */
  return ~(it.CalculateIpChecksum (hdrSize));
}

uint32_t
UdpliteHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_sourcePort = i.ReadNtohU16 ();
  m_destinationPort = i.ReadNtohU16 ();
  m_checksumCoverage = i.ReadNtohU16 ();
  if(IsLimitSet()){
    if(m_checksumCoverage<m_limit){
      m_testPass=false;//put code in send
    }
  }
  m_checksum = i.ReadU16 ();
  if (m_calcChecksum)
    {
      uint16_t headerChecksum = CalculateHeaderChecksum (start.GetSize ());
      i = start;
      uint16_t checksum = i.CalculateIpChecksum (m_checksumCoverage, headerChecksum);

      m_goodChecksum = (checksum == 0);
    }

  return GetSerializedSize ();
}
void
UdpliteHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteHtonU16 (m_sourcePort);
  i.WriteHtonU16 (m_destinationPort);
  if(m_checksumCoverage==0){
    i.WriteHtonU16 (m_payloadSize);
  }else if(m_checksumCoverage < 8||m_checksumCoverage>m_payloadSize){
    //discard packet
    i.WriteHtonU16 (m_payloadSize);
  }else {
  i.WriteHtonU16 (m_checksumCoverage);
  }/*if (m_payloadSize == 0)
    {
      i.WriteHtonU16 (start.GetSize ());
    }
  else
    {
      i.WriteHtonU16 (m_payloadSize);
    }
	*/
  if ( m_checksum == 0)
    {
      i.WriteU16 (0);

      if (m_calcChecksum)
        {
          uint16_t headerChecksum = CalculateHeaderChecksum (start.GetSize ());
          i = start;
          uint16_t checksum = i.CalculateIpChecksum (m_checksumCoverage, headerChecksum);

          i = start;
          i.Next (6);
          i.WriteU16 (checksum);
        }
    }
  else
    {
      i.WriteU16 (m_checksum);
    }


}

}