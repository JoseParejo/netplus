
#include "Observador.h"
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("first");

int
main (int argc, char *argv[])
{
  NS_LOG_INFO("Comienza");
  DataRate    tasaEnvioTV ("25Mb/s");
  DataRate	  tasaEnvioStream ("20Mb/s");
  DataRate 	  tasaEnvioVip ("30Mb/s");
//Control de línea de comandos
  CommandLine cmd;
  cmd.Parse (argc, argv);
//Resolucion en minutos o segundos?
  Time::SetResolution (Time::MIN);
//2 Nodos por contenedor, 1 nodo esta en los 3 contenedores.
  NodeContainer nodosTV;
  NodeContainer nodosPe;
  NodeContainer nodosSe;
  NodeContainer nodosVip;
  nodosTV.Create (2);
  //El nodo 0 de cada contenedor sera el cliente, el 1 el servidor
  nodosPe.Add (nodosTV.Get(0));
  nodosSe.Add (nodosTV.Get(0));
  nodosVip.Add(nodosTV.Get(0));
  nodosPe.Create (1);
  nodosSe.Create (1);
  nodosVip.Create(1);

  NS_LOG_INFO("Toca las VA");

//Para el pico de trafico de television
  Ptr <NormalRandomVariable> pton_tv = CreateObject<NormalRandomVariable> ();
  pton_tv->SetAttribute("Mean", DoubleValue(8.0));
  pton_tv->SetAttribute("Variance", DoubleValue(2.0));

//Para el pico de trafico de television
  Ptr <NormalRandomVariable> pton_vip = CreateObject<NormalRandomVariable> ();
  pton_vip->SetAttribute("Mean", DoubleValue(8.0));
  pton_vip->SetAttribute("Variance", DoubleValue(2.0));
  Ptr <NormalRandomVariable> ptoff_vip = CreateObject<NormalRandomVariable> ();
  ptoff_vip->SetAttribute("Mean", DoubleValue(8.0));
  ptoff_vip->SetAttribute("Variance", DoubleValue(2.0));

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
  NS_LOG_INFO("Comienza escenario");
//Canal P2P
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("10Gbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));
//Instalamos los nodos
  NetDeviceContainer devicesTV;
  devicesTV = pointToPoint.Install (nodosTV);
  NetDeviceContainer devicesSe;
  devicesSe = pointToPoint.Install (nodosSe);
  NetDeviceContainer devicesPe;
  devicesPe = pointToPoint.Install (nodosPe);
  NetDeviceContainer devicesVip;
  devicesVip = pointToPoint.Install (nodosVip);
  NS_LOG_INFO("Pila IP");

//Instalamos la pila IP
  InternetStackHelper stack;
  stack.Install (nodosTV);
  stack.Install (nodosPe.Get(1));
  stack.Install (nodosSe.Get(1));
  stack.Install (nodosVip.Get(1));
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
//Asignamos direcciones a servicio Premium
  address.SetBase ("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer interfacesVip = address.Assign (devicesVip);

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  NS_LOG_INFO("Comienza servers");

//Creamos la aplicacion OnOff UDP para Series
  OnOffHelper serverSe ("ns3::UdpSocketFactory", 
  	Address (InetSocketAddress(interfacesSe.GetAddress(0) ,9)));
  serverSe.SetAttribute("DataRate", DataRateValue(tasaEnvioStream));
  serverSe.SetAttribute("PacketSize", UintegerValue(1024));
  serverSe.SetAttribute("OnTime", PointerValue(pton_stream));
  serverSe.SetAttribute("OffTime", PointerValue(ptoff_stream));  

//Creamos la aplicacion OnOff UDP para Peliculas  
  OnOffHelper serverPe ("ns3::UdpSocketFactory", 
  	Address (InetSocketAddress(interfacesPe.GetAddress(0) ,10)));
  serverPe.SetAttribute("DataRate", DataRateValue(tasaEnvioStream));
  serverPe.SetAttribute("PacketSize", UintegerValue(1024));
  serverPe.SetAttribute("OnTime", PointerValue(pton_stream));
  serverPe.SetAttribute("OffTime", PointerValue(ptoff_stream));
  
//Creamos la aplicacion OnOff TCP para TV
  OnOffHelper serverTV ("ns3::TcpSocketFactory", 
  	Address (InetSocketAddress(interfacesTV.GetAddress(0) ,11)));
  serverTV.SetAttribute("DataRate", DataRateValue(tasaEnvioTV));
  serverTV.SetAttribute("PacketSize", UintegerValue(1024));
  serverTV.SetAttribute("OnTime", PointerValue(pton_tv));
  serverTV.SetAttribute("OffTime", PointerValue(ptoff_tv));

//Creamos la aplicacion OnOff TCP para TV
  OnOffHelper serverVip ("ns3::TcpSocketFactory", 
  	Address (InetSocketAddress(interfacesVip.GetAddress(0) ,11)));
  serverTV.SetAttribute("DataRate", DataRateValue(tasaEnvioVip));
  serverTV.SetAttribute("PacketSize", UintegerValue(1024));
  serverTV.SetAttribute("OnTime", PointerValue(pton_vip));
  serverTV.SetAttribute("OffTime", PointerValue(ptoff_vip));
  //serverTV.SetAttribute("MaxBytes", ));
//Instalamos las aplicaciones
  ApplicationContainer appServerSe = serverSe.Install (nodosSe.Get (1));
  ApplicationContainer appServerPe = serverPe.Install (nodosPe.Get (1));
  ApplicationContainer appServerTV = serverTV.Install (nodosTV.Get (1));
  ApplicationContainer appServerVip = serverVip.Install (nodosVip.Get (1));

//Programamos comienzo y final de las aplicaciones
  appServerPe.Start (Hours (0.0));
  appServerSe.Start (Hours (0.0));
  appServerTV.Start (Hours (0.0));
  appServerVip.Start(Hours (0.0));
  appServerPe.Stop (Hours (1.0));
  appServerSe.Stop (Hours (1.0));
  appServerTV.Stop (Hours (1.0));
  appServerVip.Stop(Hours (1.0));
  NS_LOG_INFO("Comienza clientes");

//Creamos las aplicaciones PacketSink correspondientes a las OnOff
  PacketSinkHelper clientTV ("ns3::TcpSocketFactory", 
  	Address (InetSocketAddress(interfacesTV.GetAddress(1), 11)));
  PacketSinkHelper clientSe ("ns3::UdpSocketFactory", 
  	Address (InetSocketAddress(interfacesSe.GetAddress(1), 11)));
  PacketSinkHelper clientPe ("ns3::UdpSocketFactory", 
  	Address (InetSocketAddress(interfacesPe.GetAddress(1), 11)));
  PacketSinkHelper clientVip ("ns3::TcpSocketFactory", 
  	Address (InetSocketAddress(interfacesVip.GetAddress(1), 11)));

//Instalamos las aplicaciones
  ApplicationContainer appClientSe = clientSe.Install (nodosSe.Get (0));
  ApplicationContainer appClientPe = clientPe.Install (nodosPe.Get (0));
  ApplicationContainer appClientTV = clientTV.Install (nodosTV.Get (0));
  ApplicationContainer appClientVip = clientVip.Install (nodosVip.Get (0));

//Programamos comienzo y final de las aplicaciones
  appClientTV.Start (Hours (0.3));
  appClientPe.Start (Hours (0.3));
  appClientSe.Start (Hours (0.3));
  appClientVip.Start(Hours (0.3));
  appClientSe.Stop (Hours (0.8));
  appClientPe.Stop (Hours (0.8));
  appClientTV.Stop (Hours (0.8));
  appClientVip.Stop(Hours (0.8));

  pointToPoint.EnablePcap("pruebaSerie", devicesSe.Get(1), true);
  pointToPoint.EnablePcap("pruebaPelicula", devicesPe.Get(1), true);
  pointToPoint.EnablePcap("pruebaTv", devicesTV.Get(1), true);
  pointToPoint.EnablePcap("pruebaVip", devicesVip.Get(1), true);

  NS_LOG_INFO("Comienza la simulacion");

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
