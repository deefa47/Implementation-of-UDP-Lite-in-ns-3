#ifndef UDPLITE_SOCKET_FACTORY_H
#define UDPLITE_SOCKET_FACTORY_H

#include "ns3/socket-factory.h"

namespace ns3 {

class Socket;


class UdpliteSocketFactory : public SocketFactory
{
public:
  
  static TypeId GetTypeId (void);

};

} // namespace ns3

#endif
