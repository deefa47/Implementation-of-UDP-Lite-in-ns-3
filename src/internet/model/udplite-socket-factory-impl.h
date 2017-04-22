#ifndef UDPLITE_SOCKET_FACTORY_IMPL_H
#define UDPLITE_SOCKET_FACTORY_IMPL_H

#include "ns3/udplite-socket-factory.h"
#include "ns3/ptr.h"

namespace ns3 {

class UdpliteL4Protocol;


class UdpliteSocketFactoryImpl : public UdpliteSocketFactory
{
public:
  UdpliteSocketFactoryImpl ();
  virtual ~UdpliteSocketFactoryImpl ();

  void SetUdplite (Ptr<UdpliteL4Protocol> udp);

  virtual Ptr<Socket> CreateSocket (void);

protected:
  virtual void DoDispose (void);
private:
  Ptr<UdpliteL4Protocol> m_udp; //!< the associated UDPlite L4 protocol
};

} // namespace ns3

#endif 
