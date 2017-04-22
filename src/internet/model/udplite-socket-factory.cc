
#include "udplite-socket-factory.h"
#include "ns3/uinteger.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (UdpliteSocketFactory);

TypeId UdpliteSocketFactory::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::UdpliteSocketFactory")
    .SetParent<SocketFactory> ()
    .SetGroupName ("Internet")
  ;
  return tid;
}

} // namespace ns3
