#include "udplite-header.h"
#include "udplite-l4-protocol.h"

#include "ns3/log.h"
#include "ns3/assert.h"
#include "ns3/packet.h"
#include "ns3/node.h"
#include "ns3/boolean.h"
#include "ns3/object-vector.h"
#include "ns3/ipv6.h"
#include "ns3/ipv4-route.h"
#include "ns3/ipv6-route.h"
#include "ns3/ipv6-header.h"

#include "udplite-socket-factory-impl.h"
#include "ipv4-end-point-demux.h"
#include "ipv4-end-point.h"
#include "ipv6-end-point-demux.h"
#include "ipv6-end-point.h"
#include "ipv4-l3-protocol.h"
#include "ipv6-l3-protocol.h"
#include "udp-socket-impl.h"

namespace ns3{
	
	NS_LOG_COMPONENT_DEFINE ("UdpliteL4Protocol");

	NS_OBJECT_ENSURE_REGISTERED (UdpliteL4Protocol);
	
	const uint8_t UdpliteL4Protocol::PROT_NUMBER = 136;
	TypeId
	UdpliteL4Protocol::GetTypeId (void)
	{
	  static TypeId tid = TypeId ("ns3::UdpliteL4Protocol")
	    .SetParent<IpL4Protocol> ()
	    .SetGroupName ("Internet")
	    .AddConstructor<UdpliteL4Protocol> ()
	    .AddAttribute ("SocketList", "The list of sockets associated to this protocol.",
	                   ObjectVectorValue (),
	                   MakeObjectVectorAccessor (&UdpliteL4Protocol::m_sockets),
	                   MakeObjectVectorChecker<UdpSocketImpl> ())
	  ;//check above lines--ask
	  return tid;
	}
	Ptr<Socket>
	UdpliteL4Protocol::CreateSocket (void)
	{
	  NS_LOG_FUNCTION_NOARGS ();
	  Ptr<UdpSocketImpl> socket = CreateObject<UdpSocketImpl> ();
	  socket->SetNode (m_node);
	  socket->SetUdp (this);
	  m_sockets.push_back (socket);
	  return socket;
	}
	void
UdpliteL4Protocol::NotifyNewAggregate ()
{
  NS_LOG_FUNCTION (this);
  Ptr<Node> node = this->GetObject<Node> ();
  Ptr<Ipv4> ipv4 = this->GetObject<Ipv4> ();
  Ptr<Ipv6> ipv6 = node->GetObject<Ipv6> ();

  if (m_node == 0)
    {
      if ((node != 0) && (ipv4 != 0 || ipv6 != 0))
        {
          this->SetNode (node);
          Ptr<UdpliteSocketFactoryImpl> udpliteFactory = CreateObject<UdpliteSocketFactoryImpl> ();
          udpliteFactory->SetUdp (this);
          node->AggregateObject (udpliteFactory);
        }
    }
  
  // We set at least one of our 2 down targets to the IPv4/IPv6 send
  // functions.  Since these functions have different prototypes, we
  // need to keep track of whether we are connected to an IPv4 or
  // IPv6 lower layer and call the appropriate one.
  
  if (ipv4 != 0 && m_downTarget.IsNull())
    {
      ipv4->Insert (this);
      this->SetDownTarget (MakeCallback (&Ipv4::Send, ipv4));
    }
  if (ipv6 != 0 && m_downTarget6.IsNull())
    {
      ipv6->Insert (this);
      this->SetDownTarget6 (MakeCallback (&Ipv6::Send, ipv6));
    }
  IpL4Protocol::NotifyNewAggregate ();
}

	void
	UdpliteL4Protocol::DoDispose (void)
	{
	  NS_LOG_FUNCTION_NOARGS ();
	  for (std::vector<Ptr<UdpSocketImpl> >::iterator i = m_sockets.begin (); i != m_sockets.end (); i++)
	    {
	      *i = 0;
	    }
	  m_sockets.clear ();

	  if (m_endPoints != 0)
	    {
	      delete m_endPoints;
	      m_endPoints = 0;
	    }
	  if (m_endPoints6 != 0)
	    {
	      delete m_endPoints6;
	      m_endPoints6 = 0;
	    }
	  m_node = 0;
	  m_downTarget.Nullify ();
	  m_downTarget6.Nullify ();
	/*
	 = MakeNullCallback<void,Ptr<Packet>, Ipv4Address, Ipv4Address, uint8_t, Ptr<Ipv4Route> > ();
	*/
	  IpL4Protocol::DoDispose ();
	}
	/*
	void
	UdpliteL4Protocol::Send (Ptr<Packet> packet, 
	                     Ipv4Address saddr, Ipv4Address daddr, 
	                     uint16_t sport, uint16_t dport, uint16_t checksumCoverage)
	{
	  NS_LOG_FUNCTION (this << packet << saddr << daddr << sport << dport << checksumCoverage);

	  UdpliteHeader udpliteHeader;
	  if(Node::ChecksumEnabled ())
	    {
	      udpliteHeader.EnableChecksums ();
	      udpliteHeader.InitializeChecksum (saddr,
	                                    daddr,
	                                    PROT_NUMBER);
	    }
	  udpliteHeader.SetDestinationPort (dport);
	  udpliteHeader.SetSourcePort (sport);
	  udpliteHeader.SetChecksumCoverage (checksumCoverage);

	  packet->AddHeader (udpliteHeader);

	  m_downTarget (packet, saddr, daddr, PROT_NUMBER, 0);
	}

void
UdpL4Protocol::Send (Ptr<Packet> packet, 
                     Ipv4Address saddr, Ipv4Address daddr, 
                     uint16_t sport, uint16_t dport, Ptr<Ipv4Route> route, uint16_t checksumCoverage)
{
    NS_LOG_FUNCTION (this << packet << saddr << daddr << sport << dport << checksumCoverage);

	  UdpliteHeader udpliteHeader;
	  if(Node::ChecksumEnabled ())
	    {
	      udpliteHeader.EnableChecksums ();
	      udpliteHeader.InitializeChecksum (saddr,
	                                    daddr,
	                                    PROT_NUMBER);
	    }
	  udpliteHeader.SetDestinationPort (dport);
	  udpliteHeader.SetSourcePort (sport);
	  udpliteHeader.SetChecksumCoverage (checksumCoverage);

	  packet->AddHeader (udpliteHeader);

	  m_downTarget (packet, saddr, daddr, PROT_NUMBER, route);
}

void
UdpliteL4Protocol::Send (Ptr<Packet> packet,
                     Ipv6Address saddr, Ipv6Address daddr,
                     uint16_t sport, uint16_t dport, uint16_t checksumCoverage)
{
  NS_LOG_FUNCTION (this << packet << saddr << daddr << sport << dport << checksumCoverage);

  UdpliteHeader udpliteHeader;
  if(Node::ChecksumEnabled ())
    {
      udpliteHeader.EnableChecksums ();
      udpliteHeader.InitializeChecksum (saddr,
                                    daddr,
                                    PROT_NUMBER);
    }
  udpliteHeader.SetDestinationPort (dport);
  udpliteHeader.SetSourcePort (sport);
  udpliteHeader.SetChecksumCoverage (checksumCoverage);

  packet->AddHeader (udpliteHeader);

  m_downTarget (packet, saddr, daddr, PROT_NUMBER, 0);
}
void
UdpL4Protocol::Send (Ptr<Packet> packet,
                     Ipv6Address saddr, Ipv6Address daddr,
                     uint16_t sport, uint16_t dport, Ptr<Ipv6Route> route, uint16_t checksumCoverage)
{
    NS_LOG_FUNCTION (this << packet << saddr << daddr << sport << dport << checksumCoverage);

	  UdpliteHeader udpliteHeader;
	  if(Node::ChecksumEnabled ())
	    {
	      udpliteHeader.EnableChecksums ();
	      udpliteHeader.InitializeChecksum (saddr,
	                                    daddr,
	                                    PROT_NUMBER);
	    }
	  udpliteHeader.SetDestinationPort (dport);
	  udpliteHeader.SetSourcePort (sport);
	  udpliteHeader.SetChecksumCoverage (checksumCoverage);

	  packet->AddHeader (udpliteHeader);

	  m_downTarget (packet, saddr, daddr, PROT_NUMBER, route);
	}
	enum IpL4Protocol::RxStatus
	UdpliteL4Protocol::Receive (Ptr<Packet> packet,
	                        Ipv4Header const &header,
	                        Ptr<Ipv4Interface> interface)
	{
	  NS_LOG_FUNCTION (this << packet << header);
	  UdpliteHeader udpliteHeader;
	  if(Node::ChecksumEnabled ())
	    {
	      udpliteHeader.EnableChecksums ();
	    }

	  udpliteHeader.InitializeChecksum (header.GetSource (), header.GetDestination (), PROT_NUMBER);

	  // We only peek at the header for now (instead of removing it) so that it will be intact
	  // if we have to pass it to a IPv6 endpoint via:
	  // 
	  //   UdpL4Protocol::Receive (Ptr<Packet> packet, Ipv6Address &src, Ipv6Address &dst, ...)

	  packet->PeekHeader (udpliteHeader);
	  if(!udpliteHeader.GetTestPass ())
	  {
	  	NS_LOG_INFO ("Bad checksum : dropping packet!!");
	    return IpL4Protocol::RX_CSUM_FAILED;
	  }
	  if(!udpliteHeader.IsChecksumOk ())
	    {
	      NS_LOG_INFO ("Bad checksum : dropping packet!");
	      return IpL4Protocol::RX_CSUM_FAILED;
	    }

	  NS_LOG_DEBUG ("Looking up dst " << header.GetDestination () << " port " << udpliteHeader.GetDestinationPort ()); 
	  Ipv4EndPointDemux::EndPoints endPoints =
	    m_endPoints->Lookup (header.GetDestination (), udpliteHeader.GetDestinationPort (),
	                         header.GetSource (), udpliteHeader.GetSourcePort (), interface);
	  if (endPoints.empty ())
	    {
	      if (this->GetObject<Ipv6L3Protocol> () != 0)
	        {
	          NS_LOG_LOGIC ("  No Ipv4 endpoints matched on UdpL4Protocol, trying Ipv6 "<<this);
	          Ptr<Ipv6Interface> fakeInterface;
	          Ipv6Header ipv6Header;
	          Ipv6Address src = Ipv6Address::MakeIpv4MappedAddress (header.GetSource ());
	          Ipv6Address dst = Ipv6Address::MakeIpv4MappedAddress (header.GetDestination ());
	          ipv6Header.SetSourceAddress (src);
	          ipv6Header.SetDestinationAddress (dst);
	          return (this->Receive (packet, ipv6Header, fakeInterface));
	        }

	      NS_LOG_LOGIC ("RX_ENDPOINT_UNREACH");
	      return IpL4Protocol::RX_ENDPOINT_UNREACH;
	    }

	  packet->RemoveHeader(udpliteHeader);
	  for (Ipv4EndPointDemux::EndPointsI endPoint = endPoints.begin ();
	       endPoint != endPoints.end (); endPoint++)
	    {
	      (*endPoint)->ForwardUp (packet->Copy (), header, udpliteHeader.GetSourcePort (), 
	                              interface);
	    }
	  return IpL4Protocol::RX_OK;
	}
	
	enum IpL4Protocol::RxStatus
	UdpliteL4Protocol::Receive (Ptr<Packet> packet,
	                        Ipv6Header const &header,
	                        Ptr<Ipv6Interface> interface)
	{
	  NS_LOG_FUNCTION (this << packet << header.GetSourceAddress () << header.GetDestinationAddress ());
	  UdpliteHeader udpliteHeader;
	  if(Node::ChecksumEnabled ())
	    {
	      udpliteHeader.EnableChecksums ();
	    }

	  udpliteHeader.InitializeChecksum (header.GetSourceAddress (), header.GetDestinationAddress (), PROT_NUMBER);

	  packet->RemoveHeader (udpliteHeader);

	  if(!udpliteHeader.IsChecksumOk () && !header.GetSourceAddress ().IsIpv4MappedAddress ())
	    {
	      NS_LOG_INFO ("Bad checksum : dropping packet!");
	      return IpL4Protocol::RX_CSUM_FAILED;
	    }

	  NS_LOG_DEBUG ("Looking up dst " << header.GetDestinationAddress () << " port " << udpliteHeader.GetDestinationPort ()); 
	  Ipv6EndPointDemux::EndPoints endPoints =
	    m_endPoints6->Lookup (header.GetDestinationAddress (), udpliteHeader.GetDestinationPort (),
	                         header.GetSourceAddress (), udpliteHeader.GetSourcePort (), interface);
	  if (endPoints.empty ())
	    {
	      NS_LOG_LOGIC ("RX_ENDPOINT_UNREACH");
	      return IpL4Protocol::RX_ENDPOINT_UNREACH;
	    }
	  for (Ipv6EndPointDemux::EndPointsI endPoint = endPoints.begin ();
	       endPoint != endPoints.end (); endPoint++)
	    {
	      (*endPoint)->ForwardUp (packet->Copy (), header, udpliteHeader.GetSourcePort (), interface);
	    }
	  return IpL4Protocol::RX_OK;
	}*/
}// end namespace ns3