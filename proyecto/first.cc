/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

int
main (int argc, char *argv[])
{
//Control de línea de comandos
  CommandLine cmd;
  cmd.Parse (argc, argv);
//Resolucion en minutos o segundos?
  Time::SetResolution (Time::M);
//2 Nodos por contenedor, 1 nodo esta en los 3 contenedores.
  NodeContainer nodosTV;
  NodeContainer nodosPe;
  NodeContainer nodosSe;
  nodosTV.Create (2);
  nodosPe.Add(nodosTV.Get(0));
  nodosSe.Add(nodosTV.Get(0));
  nodosPe.Create (1);
  nodosSe.Create (1);
//Para el pico de trafico de television
  Ptr <NormalRandomVariable> pton_tv = CreateObject<NormalRandomVariable> ();
  pton_tv->SetAttribute("Mean", DoubleValue(8.0));
  pton_tv->SetAttribute("Variance", DoubleValue(2.0));

  Ptr <NormalRandomVariable> ptoff_tv = CreateObject<NormalRandomVariable> ();
  ptoff_tv->SetAttribute("Mean", DoubleValue(8.0));
  ptoff_tv->SetAttribute("Variance", DoubleValue(2.0));
//Para el pico de trafico de streaming
  Ptr <NormalRandomVariable> pton_stream = CreateObject<NormalRandomVariable> ();
  pton_stream->SetAttribute("Mean", DoubleValue(8.0));
  pton_stream->SetAttribute("Variance", DoubleValue(2.0));

  Ptr <NormalRandomVariable> ptoff_stream = CreateObject<NormalRandomVariable> ();
  ptoff_stream->SetAttribute("Mean", DoubleValue(8.0));
  ptoff_stream->SetAttribute("Variance", DoubleValue(2.0));
/*	***********************
	Preparacion del escenario
	***********************
*/
//Canal P2P
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));
//Instalamos los nodos
  NetDeviceContainer devicesTV;
  devicesTV = pointToPoint.Install (nodosTV);
  NetDeviceContainer devicesPe;
  devicesSe = pointToPoint.Install (nodosPe);
  NetDeviceContainer devicesSe;
  devicesPe = pointToPoint.Install (nodosSe);

//Instalamos la pila IP
  InternetStackHelper stack;
  stack.Install (nodosTV);
  stack.Install (nodosPe);
  stack.Install (nodosSe);
//Asignamos direcciones a servicio TV
  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfacesTV = address.Assign (devicesTV);
//Asignamos direcciones a servicio Peliculas
  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer interfacesPe = address.Assign (devicesPe);
//Asignamos direcciones a servicio Serie
  address.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer interfacesSe = address.Assign (devicesSe);

//Creamos la aplicacion OnOff UDP para Series
  OnOffHelper serverSe ("ns3::UdpSocketFactory", 
  	Address (InetSocketAddres(interfacesSe.GetAddress(0) ,9)));
  serverSe.SetAttribute("DataRate", DataRateValue());
  serverSe.SetAttribute("PacketSize", UintegerValue());
  serverSe.SetAttribute("OnTime", PointerValue(pton_stream));
  serverSe.SetAttribute("OffTime", PointerValue(ptoff_stream));  

//Creamos la aplicacion OnOff UDP para Peliculas  
  OnOffHelper serverPe ("ns3::UdpSocketFactory", 
  	Address (InetSocketAddres(interfacesPe.GetAddress(0) ,10)));
  serverPe.SetAttribute("DataRate", DataRateValue());
  serverPe.SetAttribute("PacketSize", UintegerValue());
  serverPe.SetAttribute("OnTime", PointerValue(pton_stream));
  serverPe.SetAttribute("OffTime", PointerValue(ptoff_stream));
  
//Creamos la aplicacion OnOff TCP para TV
  OnOffHelper serverTV ("ns3::TcpSocketFactory", 
  	Address (InetSocketAddres(interfacesTV.GetAddress(0) ,11)));
  serverTV.SetAttribute("DataRate", DataRateValue());
  serverTV.SetAttribute("PacketSize", UintegerValue());
  serverTV.SetAttribute("OnTime", PointerValue(pton_tv));
  serverTV.SetAttribute("OffTime", PointerValue(ptoff_tv));
  //serverTV.SetAttribute("MaxBytes", UintegerValue());

//Instalamos las aplicaciones
  ApplicationContainer appServerSe = serverSe.Install (nodosSe.Get (1));
  ApplicationContainer appServerPe = serverPe.Install (nodosPe.Get (1));
  ApplicationContainer appServerTV = serverTV.Install (nodosTV.Get (1));

//Programamos comienzo y final de las aplicaciones
  //serverApps.Start (Hours (0.0));
  //serverApps.Stop (Hours (12.0));

//Creamos las aplicaciones PacketSink correspondientes a las OnOff
  PacketSinkHelper clientTV ("ns3::TcpSocketFactory", 
  	Address (InetSocketAddres(interfacesTV.GetAddress(1), 11)));
  PacketSinkHelper clientSe ("ns3::UdpSocketFactory", 
  	Address (InetSocketAddres(interfacesSe.GetAddress(1), 11)));
  PacketSinkHelper clientPe ("ns3::UdpSocketFactory", 
  	Address (InetSocketAddres(interfacesPe.GetAddress(1), 11)));

//Instalamos las aplicaciones
  ApplicationContainer appClientSe = clientSe.Install (nodosSe.Get (0));
  ApplicationContainer appClientPe = clientPe.Install (nodosPe.Get (0));
  ApplicationContainer appClientTV = clientTV.Install (nodosTV.Get (0));

//Programamos comienzo y final de las aplicaciones
  //clientApps.Start (Hours (8.0));
  //clientApps.Stop (Hours (10.0));

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
