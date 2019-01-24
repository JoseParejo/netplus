/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */



#include "Observador.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Observador");


Observador::Observador (Ptr<OnOffApplication> tcp_source_ptr, Ptr<OnOffApplication> udp_source_ptr,
		Ptr<PacketSink> tcp_sink_ptr, Ptr<PacketSink> udp_sink_ptr, std::string name)
{
	NS_LOG_FUNCTION( name << "START");
	NS_LOG_INFO( m_name << " Observador::Observador START");
	
	m_name = name;
	
	m_tcp_source_ptr = tcp_source_ptr;
	NS_LOG_DEBUG( m_name << " Fuente TCP " << m_tcp_source_ptr);

	m_udp_source_ptr = udp_source_ptr;
	NS_LOG_DEBUG( m_name << " Fuente UDP " << m_udp_source_ptr);
	
	m_paquetesTxTcp = 0;
	m_paquetesTxUdp = 0;

	m_tcp_sink_ptr = tcp_sink_ptr;
	NS_LOG_DEBUG( m_name << " Sumidero TCP " << m_tcp_sink_ptr);

	m_udp_sink_ptr = udp_sink_ptr;
	NS_LOG_DEBUG( m_name << " Sumidero UDP " << m_udp_sink_ptr);
	
	m_paquetesRxTcp = 0;
	m_paquetesRxUdp = 0;
	
	DelayJitterEstimation  m_tcpDelayJitter; 
	DelayJitterEstimation  m_udpDelayJitter; 
	
	ProgramaTrazas ();
	NS_LOG_FUNCTION( m_name << "END");
}


void Observador::DoDispose ()
{
	NS_LOG_FUNCTION( m_name << "START");
	NS_LOG_INFO( m_name << " DoDispose media retardo FINAL " << m_acumTcpDelay.Mean());
	
	Object::DoDispose ();
}


void Observador::ProgramaTrazas ()
{
	NS_LOG_FUNCTION( m_name << "START");

	m_tcp_source_ptr->TraceConnectWithoutContext ("Tx", MakeCallback (&Observador::PaqueteTxTcp, this));
	// Sólo hay tráfico UDP entre los nodos E y C
	if (m_name == "C") {
		NS_LOG_DEBUG( m_name << " Tracing UDP Tx");
		m_udp_source_ptr->TraceConnectWithoutContext ("Tx", MakeCallback (&Observador::PaqueteTxUdp, this));
	}

	m_tcp_sink_ptr->TraceConnectWithoutContext ("Rx", MakeCallback (&Observador::PaqueteRxTcp, this));
	if (m_name == "C") {
		NS_LOG_DEBUG( m_name << " Tracing UDP Rx");
		m_udp_sink_ptr->TraceConnectWithoutContext ("Rx", MakeCallback (&Observador::PaqueteRxUdp, this));
	}
	
	NS_LOG_INFO( m_name << " Todo Bien Observador::ProgramaTrazas"); 
	NS_LOG_FUNCTION( m_name << "END");
}


void Observador::PaqueteTxTcp (Ptr<const Packet>pqt_ptr)
{
	NS_LOG_FUNCTION( m_name << pqt_ptr);
	double now = Simulator::Now().GetDouble();
	
	NS_LOG_DEBUG( m_name << " PrepareTxTcp");
	m_tcpDelayJitter.PrepareTx (pqt_ptr);
	m_paquetesTxTcp++;
	NS_LOG_DEBUG( m_name << " TxTcp " << now);
	NS_LOG_FUNCTION( m_name << "END");
}

void Observador::PaqueteTxUdp (Ptr<const Packet>pqt_ptr)
{
	NS_LOG_FUNCTION( m_name << pqt_ptr);
	double now = Simulator::Now().GetDouble();
	
	NS_LOG_DEBUG( m_name << " PrepareTxUdp");
	m_udpDelayJitter.PrepareTx (pqt_ptr);
	m_paquetesTxUdp++;
	NS_LOG_DEBUG( m_name << " TxUdp " << now);
	NS_LOG_FUNCTION( m_name << "END");
}


void Observador::PaqueteRxTcp (Ptr<const Packet> pqt_ptr, const Address &address)
{
	NS_LOG_FUNCTION( m_name << pqt_ptr << address << pqt_ptr->GetSize() );
	
	m_tcpDelayJitter.RecordRx (pqt_ptr);	
	m_paquetesRxTcp++;
	NS_LOG_DEBUG( m_name << " RxTcp LastJitter: " << m_tcpDelayJitter.GetLastJitter() <<
			", LastDelay: " << m_tcpDelayJitter.GetLastDelay() <<
			", packet #" << m_paquetesRxTcp
			);
	
	m_acumTcpDelay.Update(m_tcpDelayJitter.GetLastDelay().GetDouble() );
	m_acumTcpJitter.Update(m_tcpDelayJitter.GetLastJitter() );
	
	NS_LOG_FUNCTION( m_name << "END");
}


void Observador::PaqueteRxUdp (Ptr<const Packet> pqt_ptr, const Address &address)
{
	NS_LOG_FUNCTION( m_name << pqt_ptr << address << pqt_ptr->GetSize() );
	
	m_udpDelayJitter.RecordRx (pqt_ptr);
	m_paquetesRxUdp++;
	NS_LOG_DEBUG( m_name << " RxUdp LastJitter: " << m_udpDelayJitter.GetLastJitter() <<
			", LastDelay: " << m_udpDelayJitter.GetLastDelay() <<
			", packet #" << m_paquetesRxUdp
			);
	
	m_acumUdpDelay.Update(m_udpDelayJitter.GetLastDelay().GetDouble() );
	m_acumUdpJitter.Update(m_udpDelayJitter.GetLastJitter() );

	NS_LOG_FUNCTION( m_name << "END");
}


double Observador::GetTcpRxCorrectos()
{
	NS_LOG_FUNCTION( m_name << m_paquetesRxTcp << m_paquetesTxTcp);
	NS_LOG_DEBUG( m_name << " GetTcpRxCorrectos " << m_paquetesRxTcp << " / "<< m_paquetesTxTcp);
	return m_paquetesTxTcp ? (double) m_paquetesRxTcp/m_paquetesTxTcp : std::numeric_limits<double>::quiet_NaN();
}


double Observador::GetUdpRxCorrectos()
{
	NS_LOG_FUNCTION( m_name << m_paquetesRxUdp << m_paquetesTxUdp);
	NS_LOG_DEBUG( m_name << " GetUdpRxCorrectos " << m_paquetesRxUdp << " / "<< m_paquetesTxUdp);
	return m_paquetesTxUdp ? (double) m_paquetesRxUdp/m_paquetesTxUdp : std::numeric_limits<double>::quiet_NaN();
}


double Observador::GetTcpDelayMean()
{
	NS_LOG_FUNCTION( m_name << m_acumTcpDelay.Mean());
	return m_acumTcpDelay.Mean();
}


double Observador::GetTcpJitterMean()
{
	NS_LOG_FUNCTION( m_name << m_acumTcpJitter.Mean());
	return m_acumTcpJitter.Mean();
}


double Observador::GetUdpDelayMean()
{
	NS_LOG_FUNCTION( m_name << m_acumUdpDelay.Mean());
	return m_acumUdpDelay.Mean();
}


double Observador::GetUdpJitterMean()
{
	NS_LOG_FUNCTION( m_name << m_acumUdpJitter.Mean());
	return m_acumUdpJitter.Mean();
}


