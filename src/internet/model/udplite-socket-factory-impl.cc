#include "udplite-socket-factory-impl.h"
#include "udplite-l4-protocol.h"
#include "ns3/socket.h"
#include "ns3/assert.h"

namespace ns3 {

UdpliteSocketFactoryImpl::UdpliteSocketFactoryImpl ()
  : m_udp (0)
{
}
UdpliteSocketFactoryImpl::~UdpliteSocketFactoryImpl ()
{
  NS_ASSERT (m_udp == 0);
}

void
UdpliteSocketFactoryImpl::SetUdplite (Ptr<UdpliteL4Protocol> udp)
{
  m_udp = udp;
}

Ptr<Socket>
UdpliteSocketFactoryImpl::CreateSocket (void)
{
  return m_udp->CreateSocket ();
}

void 
UdpliteSocketFactoryImpl::DoDispose (void)
{
  m_udp = 0;
  UdpliteSocketFactory::DoDispose ();
}

} // namespace ns3
