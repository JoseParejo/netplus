/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */



#ifndef OBSERVADOR_H
#define OBSERVADOR_H

#include <iostream>
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/callback.h"
#include "ns3/csma-module.h"
#include "ns3/average.h"

#include "ns3/point-to-point-module.h"
#include "ns3/tcp-socket-base.h"
#include "ns3/applications-module.h"

#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include <ns3/gnuplot.h>

#include "ns3/delay-jitter-estimation.h"

using namespace ns3;

class Observador : public Object
{
public:  
	Observador (Ptr<OnOffApplication> tcp_source_ptr, Ptr<OnOffApplication> udp_source_ptr,
			Ptr<PacketSink> tcp_sink_ptr, Ptr<PacketSink> udp_sink_ptr, std::string name);
	void DoDispose (); 	

	double GetTcpRxCorrectos();
	double GetUdpRxCorrectos();
	
	double GetTcpDelayMean();
	
	double GetTcpJitterMean();
	
	double GetUdpDelayMean();
	
	double GetUdpJitterMean();

private:  
	void ProgramaTrazas ();
	void PaqueteTxTcp (Ptr<const Packet> pqt);
	void PaqueteTxUdp (Ptr<const Packet> pqt);
	void PaqueteRxTcp (Ptr<const Packet> pqt, const Address &address);
	void PaqueteRxUdp (Ptr<const Packet> pqt, const Address &address);
	
	Ptr<OnOffApplication> m_tcp_source_ptr;
	Ptr<OnOffApplication> m_udp_source_ptr;

	Ptr<PacketSink> m_tcp_sink_ptr;
	Ptr<PacketSink> m_udp_sink_ptr;

	uint64_t m_paquetesTxTcp;
	uint64_t m_paquetesTxUdp;

	uint64_t m_paquetesRxTcp;
	uint64_t m_paquetesRxUdp;

	DelayJitterEstimation  m_tcpDelayJitter; 
	DelayJitterEstimation  m_udpDelayJitter; 

	Average<double> m_acumTcpDelay;
	Average<double> m_acumTcpJitter;
	Average<double> m_acumUdpDelay;
	Average<double> m_acumUdpJitter;
	
	std::string m_name;
};

#endif
