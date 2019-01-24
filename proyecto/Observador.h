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
	Observador::Observador (
		Ptr<OnOffApplication> vip_source_ptr,Ptr<OnOffApplication> tv_source_ptr, 
		Ptr<OnOffApplication> pel_source_ptr, Ptr<OnOffApplication> ser_source_ptr,
		Ptr<PacketSink> vip_sink_ptr, Ptr<PacketSink> tv_sink_ptr,
		Ptr<PacketSink> pel_sink_ptr, Ptr<PacketSink> ser_sink_ptr); 	

		//Funciones necesarias para la obtención de las variables privadas del nodo para gestionar las estadísticas en el programa principal	
	double  	GetRetardoMedio ();			
	double 		GetVariacionMedia ();
	double 		GetPorcentajeCorrecto ();

private:  
	void ProgramaTrazas ();

	void PaqueteEntregadoVip (Ptr<const Packet> pqt);
	void PaqueteEntregadoTV (Ptr<const Packet> pqt);
	void PaqueteEntregadoPel (Ptr<const Packet> pqt);
	void PaqueteEntregadoSer (Ptr<const Packet> pqt);
	void PaqueteRecibidoVip (Ptr<const Packet> pqt);
	void PaqueteRecibidoTV (Ptr<const Packet> pqt);
	void PaqueteRecibidoPel (Ptr<const Packet> pqt);
	void PaqueteRecibidoSer (Ptr<const Packet> pqt);
	
	Ptr<OnOffApplication> server_vip;
	Ptr<OnOffApplication> server_tv;
	Ptr<OnOffApplication> server_pel;
	Ptr<OnOffApplication> server_ser;

	Ptr<PacketSink> client_vip;
	Ptr<PacketSink> client_tv;
	Ptr<PacketSink> client_pel;
	Ptr<PacketSink> client_ser;

	uint64_t m_paquetesTxVip;
	uint64_t m_paquetesTxTv;
	uint64_t m_paquetesTxPel;
	uint64_t m_paquetesTxSer;

	uint64_t m_paquetesRxVip;
	uint64_t m_paquetesRxTv;
	uint64_t m_paquetesRxPel;
	uint64_t m_paquetesRxSer;

	// Tiempo de simulacion real, definido por la recepcion del ultimo paquete
	Time ultimo_retardo_vip;
	Time ultimo_retardo_tv;
	Time ultimo_retardo_pel;
	Time ultimo_retardo_ser;
		
	// Variables necesaria para calcular el retardo medio y la variacion de retardo
	Average <double> m_retardos_vip;
	Average <double> m_retardos_tv;
	Average <double> m_retardos_pel;
	Average <double> m_retardos_ser;

	Average <double> m_variacion_retardos_vip;
	Average <double> m_variacion_retardos_tv;
	Average <double> m_variacion_retardos_pel;
	Average <double> m_variacion_retardos_ser;

	// Mapa donde ser guarda el uid del paquete y el retardo 
	std::map<SequenceNumber32, Time> Envios;
	std::map<uint64_t, Time> EnviosUDP;
};

#endif
		