///* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "Observador.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Observador");

typedef struct porcentaje {
	double porcentaje_vip;
	double porcentaje_tv;
	double porcentaje_stream;
}PORCENTAJE;

typedef struct mediaRetardo {
	double mediaRetardo_vip;
	double mediaRetardo_tv;
	double mediaRetardo_stream;
}MEDIARETARDO;

typedef struct variacionRetardo {
	double variacionRetardo_vip;
	double variacionRetardo_tv;
	double variacionRetardo_stream;
}VARIACIONRETARDO;


Observador::Observador (
	Ptr<OnOffApplication> vip_source_ptr,Ptr<OnOffApplication> tv_source_ptr, 
	Ptr<OnOffApplication> pel_source_ptr, Ptr<OnOffApplication> ser_source_ptr,
	Ptr<PacketSink> vip_sink_ptr, Ptr<PacketSink> tv_sink_ptr,
	Ptr<PacketSink> pel_sink_ptr, Ptr<PacketSink> ser_sink_ptr)
{
	NS_LOG_FUNCTION("START");
	NS_LOG_INFO(" Observador::Observador START");
	
	
	server_vip = vip_source_ptr;
	NS_LOG_DEBUG(" Fuente Vip " << server_vip);

	server_tv = tv_source_ptr;
	NS_LOG_DEBUG(" Fuente TV " << server_tv);

	server_pel = pel_source_ptr;
	NS_LOG_DEBUG(" Fuente Peliculas " << server_pel);

	server_ser = ser_source_ptr;
	NS_LOG_DEBUG(" Fuente Series " << server_ser);
	
	m_paquetesTxVip = 0;
	m_paquetesTxTv = 0;
	m_paquetesTxPe = 0;
	m_paquetesTxSe = 0;

	client_vip = vip_sink_ptr;
	NS_LOG_DEBUG(" Sumidero Vip " << client_vip);
	
	client_tv = tv_sink_ptr;
	NS_LOG_DEBUG(" Sumidero TV " << client_tv);
	
	client_pel = pel_sink_ptr;
	NS_LOG_DEBUG(" Sumidero Peliculas " << client_pel);
	
	client_ser = ser_sink_ptr;
	NS_LOG_DEBUG(" Sumidero Series " << client_ser);

	m_paquetesRxVip = 0;
	m_paquetesRxTv = 0;
	m_paquetesRxPe = 0;
	m_paquetesRxSe = 0;

	ultimo_retardo_vip = Time("0s");
	m_retardos_vip.Reset();
	m_variacion_retardos_vip.Reset();

	ultimo_retardo_tv = Time("0s");
	m_retardos_tv.Reset();
	m_variacion_retardos_tv.Reset();

	ultimo_retardo_pel = Time("0s");
	m_retardos_pel.Reset();
	m_variacion_retardos_pel.Reset();

	ultimo_retardo_ser = Time("0s");
	m_retardos_ser.Reset();
	m_variacion_retardos_ser.Reset();
	
	ProgramaTrazas ();
	NS_LOG_FUNCTION("END");
}

//Funcion que programa las trazas necesarias para la gestion del canal UDP.
void Observador::ProgramaTrazas ()
{
   NS_LOG_FUNCTION("ProgramaTrazas");
   server_vip->TraceConnectWithoutContext ("Tx", MakeCallback (&Observador::PaqueteEntregadoVip, this));
   client_vip->TraceConnectWithoutContext ("Rx", MakeCallback (&Observador::PaqueteRecibidoVip, this));
   server_tv->TraceConnectWithoutContext ("Tx", MakeCallback (&Observador::PaqueteEntregadoTV, this));
   client_tv->TraceConnectWithoutContext ("Rx", MakeCallback (&Observador::PaqueteRecibidoTV, this));
   server_pel->TraceConnectWithoutContext ("Tx", MakeCallback (&Observador::PaqueteEntregadoPel, this));
   client_pel->TraceConnectWithoutContext ("Rx", MakeCallback (&Observador::PaqueteRecibidoPel, this));
   server_ser->TraceConnectWithoutContext ("Tx", MakeCallback (&Observador::PaqueteEntregadoSer, this));
   client_ser->TraceConnectWithoutContext ("Rx", MakeCallback (&Observador::PaqueteRecibidoSer, this));
}

//Funcion que gestiona la traza Tx del nodo sumidero
void Observador::PaqueteEntregadoVip (Ptr<const Packet> pqt)
{
	NS_LOG_FUNCTION_NOARGS();
	
	TcpHeader header;
	pqt->PeekHeader(header);
	
	SequenceNumber32 aux_secuencia = header.GetSequenceNumber();
    	
	//Realizamos una búsqueda del paquete en el map
    std::map<SequenceNumber32, Time>::iterator Iterador=Envios.find(aux_secuencia);    
    

    if(Iterador == Envios.end()) {
       
        //Si el paquete no se encuentra registrado en el map, se crea una fila para llevar la cuenta del numero de intentos
        NS_LOG_DEBUG("[NUEVO ENVIO] Paquete no encontrado en el map, n secuencia" << aux_secuencia << "[tx = " << m_paquetesTxVip << "]");
        Envios [aux_secuencia] = (Simulator::Now()); 
    } 

    else {   
        //El uid se encuentra en la tabla, por lo que el paquete ya ha sido registrado
        //De manera que incrementamos el numero de intentos en 1
        NS_LOG_DEBUG("[REENVIO] Paquete encontrado en el map, n secuencia" << aux_secuencia<< "[tx = " << m_paquetesTxVip << "]");
    }
}

//Funcion que gestiona la traza Tx del nodo sumidero
void Observador::PaqueteEntregadoTV (Ptr<const Packet> pqt)
{
	NS_LOG_FUNCTION_NOARGS();
	
	TcpHeader header;
	pqt->PeekHeader(header);
	
	SequenceNumber32 aux_secuencia = header.GetSequenceNumber();
    	
	//Realizamos una búsqueda del paquete en el map
    std::map<SequenceNumber32, Time>::iterator Iterador=Envios.find(aux_secuencia);    
    

    if(Iterador == Envios.end()) {
       
        //Si el paquete no se encuentra registrado en el map, se crea una fila para llevar la cuenta del numero de intentos
        NS_LOG_DEBUG("[NUEVO ENVIO] Paquete no encontrado en el map, n secuencia" << aux_secuencia << "[tx = " << m_paquetesTxTv << "]");
        Envios [aux_secuencia] = (Simulator::Now()); 
    } 

    else {   
        //El uid se encuentra en la tabla, por lo que el paquete ya ha sido registrado
        //De manera que incrementamos el numero de intentos en 1
        NS_LOG_DEBUG("[REENVIO] Paquete encontrado en el map, n secuencia" << aux_secuencia<< "[tx = " << m_paquetesTxTv << "]");
    }
}

//Funcion que indica que se ha enviado un paquete por el canal UDP.
void Observador::PaqueteEntregadoPel (Ptr<const Packet> pqt)
{
	uint64_t uid_enviado;
	NS_LOG_FUNCTION("PaqueteEntregado");
	NS_LOG_DEBUG("Envio de paquete: " << pqt->GetUid() <<" en el instante: " << Simulator::Now () );
	
	//Obtenemos el uid del paquete que enviamos y lo almacenamos en una tabla, junto con el instante en el que se envia
	uid_enviado = pqt->GetUid();
	
	EnviosUDP [uid_enviado] = Simulator::Now(); //guardamos el tiempo en la tabla con indice el identificador del paquete
	
	//Incrementamos el numero de paquetes enviado para calcular estadísticos
	m_paquetesTxPe++;
}

//Funcion que indica que se ha enviado un paquete por el canal UDP.
void Observador::PaqueteEntregadoSer (Ptr<const Packet> pqt)
{
	uint64_t uid_enviado;
	NS_LOG_FUNCTION("PaqueteEntregado");
	NS_LOG_DEBUG("Envio de paquete: " << pqt->GetUid() <<" en el instante: " << Simulator::Now () );
	
	//Obtenemos el uid del paquete que enviamos y lo almacenamos en una tabla, junto con el instante en el que se envia
	uid_enviado = pqt->GetUid();
	
	EnviosUDP [uid_enviado] = Simulator::Now(); //guardamos el tiempo en la tabla con indice el identificador del paquete
	
	//Incrementamos el numero de paquetes enviado para calcular estadísticos
	m_paquetesTxSe++;
}

//Funcion que gestiona la traza Rx del nodo sumidero
void Observador::PaqueteRecibidoVip (Ptr<const Packet> pqt)
{
	NS_LOG_FUNCTION_NOARGS();
	
	TcpHeader header;
	pqt->PeekHeader(header);
	
	SequenceNumber32 aux_secuencia = header.GetSequenceNumber();
	
	//Realizamos una búsqueda del paquete en el map
    std::map<SequenceNumber32, Time>::iterator Iterador=Envios.find(aux_secuencia);    
    
    if(Iterador == Envios.end()) {
        //Si el paquete no se encuentra registrado en el map, se crea una fila para llevar la cuenta del numero de intentos
        NS_LOG_DEBUG("[ERROR] Paquete no encontrado en el map, numero de secuencia" << aux_secuencia );
    } 
    else {  
        //El uid se encuentra en la tabla, por lo que el paquete ya ha sido registrado
        //De manera que incrementamos el numero de intentos en 1
        NS_LOG_DEBUG("[PAQUETE RECIBIDO] numero de secuencia = " << aux_secuencia);
		
		Time retardo_vip = (Simulator::Now()) - Envios[aux_secuencia];
		m_retardos_vip.Update (retardo_vip.GetMilliSeconds());
		Envios.erase(Iterador);
			
		if(m_numPaqRx > 1)
			m_variacion_retardos_vip.Update(abs(ultimo_retardo_vip.GetMilliSeconds() - retardo_vip.GetMilliSeconds()));
	
		//Actualizamos "ultimo_retardo" para el siguiente paquete
		ultimo_retardo_vip = retardo_vip;		
    }
}

//Funcion que gestiona la traza MacRx del nodo sumidero
void Observador::PaqueteRecibidoTV (Ptr<const Packet> pqt)
{
	NS_LOG_FUNCTION_NOARGS();
	
	TcpHeader header;
	pqt->PeekHeader(header);
	
	SequenceNumber32 aux_secuencia = header.GetSequenceNumber();
	
	//Realizamos una búsqueda del paquete en el map
    std::map<SequenceNumber32, Time>::iterator Iterador=Envios.find(aux_secuencia);    
    
    if(Iterador == Envios.end()) {
        //Si el paquete no se encuentra registrado en el map, se crea una fila para llevar la cuenta del numero de intentos
        NS_LOG_DEBUG("[ERROR] Paquete no encontrado en el map, numero de secuencia" << aux_secuencia );
    } 
    else {  
        //El uid se encuentra en la tabla, por lo que el paquete ya ha sido registrado
        //De manera que incrementamos el numero de intentos en 1
        NS_LOG_DEBUG("[PAQUETE RECIBIDO] numero de secuencia = " << aux_secuencia);
		
		Time retardo_tv = (Simulator::Now()) - Envios[aux_secuencia];
		m_retardos_tv.Update (retardo_tv.GetMilliSeconds());
		Envios.erase(Iterador);
			
		if(m_numPaqRx > 1)
			m_variacion_retardos_tv.Update(abs(ultimo_retardo_tv.GetMilliSeconds() - retardo_tv.GetMilliSeconds()));
	
		//Actualizamos "ultimo_retardo" para el siguiente paquete
		ultimo_retardo_tv = retardo_tv;		
    }
}

//Funcion que indica que se ha recibido un paquete por el canal UDP.
void Observador::PaqueteRecibidoSer (Ptr<const Packet> pqt, const Address &)
{
	NS_LOG_FUNCTION("Observador:PaqueteRecibido");
	uint64_t uid_recibido = pqt->GetUid ();
	
	//Obtenemos el uid del paquete que hemos recibido y lo buscamos en la tabla del nodo
	std::map<uint64_t, Time>::iterator Comparador=EnviosUDP.find(uid_recibido);    //Buscamos en la tabla el uid
	
	if(Comparador == EnviosUDP.end()) {
		//Si el comparador señala al final de la tabla, esque no hemos encontrado el uid, y el paquete no lo ha enviado este nodo
		NS_LOG_DEBUG("ERROR " << uid_recibido << " no encontrado en el sumidero");
	} 
	
	else {  	
		//El uid se encuentra en la tabla, por lo que el paquete lo envió este nodo
		//Calculamos el retardo
		Time retardo_ser = (Simulator::Now() -EnviosUDP[uid_recibido]);    //si lo encontramos calculamos el retardo
		
		//Retiramos la tupla en la que se encontraba este paquete
		EnviosUDP.erase(Comparador); 
		
		//Incrementamos el numero de paquetes recibidos correctamente para calcular los estadísticos
		m_numPaqRx++;
		NS_LOG_DEBUG("Recepcion de paquete ENCONTRADO: " << uid_recibido);
		
		//Si existe retardo, lo almacenamos para posteriormente calcular el estadistico
		NS_LOG_DEBUG("Instante: " << Simulator::Now() << " Retardo calculado " << retardo_ser << " para el paquete: " << uid_recibido);
		m_retardos_ser.Update (retardo_ser.GetMilliSeconds());
		
		if(m_numPaqRx > 1)
			m_variacion_retardos_ser.Update(abs(ultimo_retardo_ser.GetMilliSeconds() - retardo_ser.GetMilliSeconds()));
	
		//Actualizamos "ultimo_retardo" para el siguiente paquete
		ultimo_retardo_ser = retardo_ser;
	}
}

//Funcion que indica que se ha recibido un paquete por el canal UDP.
void Observador::PaqueteRecibidoPel (Ptr<const Packet> pqt, const Address &)
{
	NS_LOG_FUNCTION("Observador:PaqueteRecibido");
	uint64_t uid_recibido = pqt->GetUid ();
	
	//Obtenemos el uid del paquete que hemos recibido y lo buscamos en la tabla del nodo
	std::map<uint64_t, Time>::iterator Comparador=EnviosUDP.find(uid_recibido);    //Buscamos en la tabla el uid
	
	if(Comparador == EnviosUDP.end()) {
		//Si el comparador señala al final de la tabla, esque no hemos encontrado el uid, y el paquete no lo ha enviado este nodo
		NS_LOG_DEBUG("ERROR " << uid_recibido << " no encontrado en el sumidero");
	} 
	
	else {  	
		//El uid se encuentra en la tabla, por lo que el paquete lo envió este nodo
		//Calculamos el retardo
		Time retardo_pel = (Simulator::Now() -EnviosUDP[uid_recibido]);    //si lo encontramos calculamos el retardo
		
		//Retiramos la tupla en la que se encontraba este paquete
		EnviosUDP.erase(Comparador); 
		
		//Incrementamos el numero de paquetes recibidos correctamente para calcular los estadísticos
		m_paquetesRxPe++;
		NS_LOG_DEBUG("Recepcion de paquete ENCONTRADO: " << uid_recibido);
		
		//Si existe retardo, lo almacenamos para posteriormente calcular el estadistico
		NS_LOG_DEBUG("Instante: " << Simulator::Now() << " Retardo calculado " << retardo_pel << " para el paquete: " << uid_recibido);
		m_retardos_pel.Update (retardo_pel.GetMilliSeconds());
		
		if(m_paquetesRxPe > 1)
			m_variacion_retardos_pel.Update(abs(ultimo_retardo_pel.GetMilliSeconds() - retardo_pel.GetMilliSeconds()));
	
		//Actualizamos "ultimo_retardo" para el siguiente paquete
		ultimo_retardo_pel = retardo_pel;
	}
}

//Funcion que devuelve el porcentaje de paquetes correctos en la comunicacion entre los nodos C y E.
double Observador::GetPorcentajeCorrecto ()
{
	PORCENTAJE porcentaje;

	NS_LOG_INFO ("Porcentaje referido a servidor VIP");
	NS_LOG_INFO("Rx = " << m_paquetesRxVip << " y Tx = " << m_paquetesTxVip);
	porcentaje.porcentaje_vip = ((double)m_paquetesRxVip / ((double) m_paquetesTxVip))*100;

	NS_LOG_INFO ("Porcentaje referido a servidor TV");
	NS_LOG_INFO("Rx = " << m_paquetesRxTv << " y Tx = " << m_paquetesTxTv);
	porcentaje.porcentaje_tv = ((double)m_paquetesRxTv / ((double) m_paquetesTxTv))*100;

	NS_LOG_INFO ("Porcentaje referido a servidor STREAMING");
	NS_LOG_INFO("Rx = " << (m_paquetesRxSe + m_paquetesRxPe) << " y Tx = " << (m_paquetesTxSe + m_paquetesTxPe));
	porcentaje.porcentaje_stream = (((double)m_paquetesRxSe + (double)m_paquetesRxPe) / (((double) m_paquetesTxSe) + (double)m_paquetesTxPe))*100;
	
	if (porcentaje_vip > 100)
		porcentaje_vip = 100;

	if (porcentaje_tv > 100)
		porcentaje_tv = 100;

	if (porcentaje_stream > 100)
		porcentaje_stream = 100;

	return porcentaje;
}

//Funcion que devuelve el retardo medio calculado a lo largo de la simulacion
double Observador::GetRetardoMedio ()
{	
	MEDIARETARDO retardos;

	retardos.mediaRetardo_vip = m_retardos_vip.Mean(); 
	retardos.mediaRetardo_tv = m_retardos_tv.Mean();
	retardos.mediaRetardo_stream = m_retardos_pel.Mean() + m_retardos_ser.Mean();

	return retardos;
}

//Funcion que devuelve el valor medio de la variacion de retardo calculada a lo largo de la simulacion
double Observador::GetVariacionMedia ()
{
	VARIACIONRETARDO variacion;

	variacion.variacionRetardo_vip = m_variacion_retardos_vip.Mean(); 
	variacion.variacionRetardo_tv = m_variacion_retardos_tv.Mean();
	variacion.variacionRetardo_stream = m_variacion_retardos_pel.Mean() + m_variacion_retardos_ser.Mean();

	return variacion;
}