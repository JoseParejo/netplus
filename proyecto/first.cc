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
  CommandLine cmd;
  cmd.Parse (argc, argv);
  
  Time::SetResolution (Time::US);

  NodeContainer nodosTV;
  NodeContainer nodosPe;
  NodeContainer nodosSe;
  nodosTV.Create (2);
  nodosPe.Add(nodosTV.Get(0));
  nodosSe.Add(nodosTV.Get(0));
  nodosPe.Create (1);
  nodosSe.Create (1);



  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer devicesTV;
  devicesTV = pointToPoint.Install (nodosTV);
  NetDeviceContainer devicesPe;
  devicesSe = pointToPoint.Install (nodosPe);
  NetDeviceContainer devicesSe;
  devicesPe = pointToPoint.Install (nodosSe);

  InternetStackHelper stack;
  stack.Install (nodosTV);
  stack.Install (nodosPe);
  stack.Install (nodosSe);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfacesTV = address.Assign (devicesTV);

  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer interfacesPe = address.Assign (devicesPe);

  address.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer interfacesSe = address.Assign (devicesSe);

  UdpEchoServerHelper serverSe (9);
  UdpEchoServerHelper serverPe (10);
  OnOffHelper serverTV ("ns3::TcpSocketFactory", 
  	Address (InetSocketAddres(interfacesTV.GetAddress(0) ,11)));

  ApplicationContainer appServerSe = serverSe.Install (nodosSe.Get (1));
  ApplicationContainer appServerPe = serverPe.Install (nodosPe.Get (1));
  ApplicationContainer appServerTV = serverTV.Install (nodosTV.Get (1));

  //serverApps.Start (Seconds (1.0));
  //serverApps.Stop (Seconds (10.0));

  UdpEchoClientHelper clientSe (interfacesSe.GetAddress (1), 9);
  clientSe.SetAttribute ("MaxPackets", UintegerValue (1));
  clientSe.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  clientSe.SetAttribute ("PacketSize", UintegerValue (1024));
  
  UdpEchoClientHelper clientPe (interfacesPe.GetAddress (1), 10);
  clientPe.SetAttribute ("MaxPackets", UintegerValue (1));
  clientPe.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  clientPe.SetAttribute ("PacketSize", UintegerValue (1024));

  PacketSinkHelper clientTV ("ns3::TcpSocketFactory", 
  	Address (InetSocketAddres(interfacesTV.GetAddress(1), 11)));

  ApplicationContainer appClientSe = clientSe.Install (nodosSe.Get (0));
  ApplicationContainer appClientPe = clientPe.Install (nodosPe.Get (0));
  ApplicationContainer appClientTV = clientTV.Install (nodosTV.Get (0));

  //clientApps.Start (Seconds (2.0));
  //clientApps.Stop (Seconds (10.0));

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
