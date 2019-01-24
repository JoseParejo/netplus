/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */



#include "Observador.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Observador");


Observador::Observador (
	Ptr<OnOffApplication> vip_source_ptr,Ptr<OnOffApplication> tv_source_ptr, 
	Ptr<OnOffApplication> pel_source_ptr, Ptr<OnOffApplication> ser_source_ptr,
	Ptr<PacketSink> vip_sink_ptr, Ptr<PacketSink> tv_sink_ptr,
	Ptr<PacketSink> pel_sink_ptr, Ptr<PacketSink> ser_sink_ptr)
{
	NS_LOG_FUNCTION("START");
	NS_LOG_INFO(" Observador::Observador START");
	
	
	m_vip_source_ptr = vip_source_ptr;
	NS_LOG_DEBUG(" Fuente Vip " << m_vip_source_ptr);

	m_tv_source_ptr = tv_source_ptr;
	NS_LOG_DEBUG(" Fuente TV " << m_tv_source_ptr);

	m_pel_source_ptr = pel_source_ptr;
	NS_LOG_DEBUG(" Fuente Peliculas " << m_pel_source_ptr);

	m_ser_source_ptr = ser_source_ptr;
	NS_LOG_DEBUG(" Fuente Series " << m_ser_source_ptr);
	
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

//Funcion que gestiona la traza MacTx del nodo sumidero
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
//Funcion que gestiona la traza MacTx del nodo sumidero
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

//Funcion que gestiona la traza MacRx del nodo sumidero
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
		
		Time retardo = (Simulator::Now()) - Envios[aux_secuencia];
		m_retardos.Update (retardo.GetMilliSeconds());
		Envios.erase(Iterador);
			
		if(m_numPaqRx > 1)
			m_variacion_retardos.Update(abs(ultimo_retardo.GetMilliSeconds() - retardo.GetMilliSeconds()));
	
		//Actualizamos "ultimo_retardo" para el siguiente paquete
		ultimo_retardo = retardo;		
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
		
		Time retardo = (Simulator::Now()) - Envios[aux_secuencia];
		m_retardos.Update (retardo.GetMilliSeconds());
		Envios.erase(Iterador);
			
		if(m_numPaqRx > 1)
			m_variacion_retardos.Update(abs(ultimo_retardo.GetMilliSeconds() - retardo.GetMilliSeconds()));
	
		//Actualizamos "ultimo_retardo" para el siguiente paquete
		ultimo_retardo = retardo;		
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
		Time retardo = (Simulator::Now() -EnviosUDP[uid_recibido]);    //si lo encontramos calculamos el retardo
		
		//Retiramos la tupla en la que se encontraba este paquete
		EnviosUDP.erase(Comparador); 
		
		//Incrementamos el numero de paquetes recibidos correctamente para calcular los estadísticos
		m_numPaqRx++;
		NS_LOG_DEBUG("Recepcion de paquete ENCONTRADO: " << uid_recibido);
		
		//Si existe retardo, lo almacenamos para posteriormente calcular el estadistico
		NS_LOG_DEBUG("Instante: " << Simulator::Now() << " Retardo calculado " << retardo << " para el paquete: " << uid_recibido);
		m_retardos.Update (retardo.GetMilliSeconds());
		
		if(m_numPaqRx > 1)
			m_variacion_retardos.Update(abs(ultimo_retardo.GetMilliSeconds() - retardo.GetMilliSeconds()));
	
		//Actualizamos "ultimo_retardo" para el siguiente paquete
		ultimo_retardo = retardo;
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
		Time retardo = (Simulator::Now() -EnviosUDP[uid_recibido]);    //si lo encontramos calculamos el retardo
		
		//Retiramos la tupla en la que se encontraba este paquete
		EnviosUDP.erase(Comparador); 
		
		//Incrementamos el numero de paquetes recibidos correctamente para calcular los estadísticos
		m_paquetesRxPe++;
		NS_LOG_DEBUG("Recepcion de paquete ENCONTRADO: " << uid_recibido);
		
		//Si existe retardo, lo almacenamos para posteriormente calcular el estadistico
		NS_LOG_DEBUG("Instante: " << Simulator::Now() << " Retardo calculado " << retardo << " para el paquete: " << uid_recibido);
		m_retardos.Update (retardo.GetMilliSeconds());
		
		if(m_paquetesRxPe > 1)
			m_variacion_retardos.Update(abs(ultimo_retardo.GetMilliSeconds() - retardo.GetMilliSeconds()));
	
		//Actualizamos "ultimo_retardo" para el siguiente paquete
		ultimo_retardo = retardo;
	}
}

//Funcion que devuelve el porcentaje de paquetes correctos en la comunicacion entre los nodos C y E.
double Observador::GetPorcentajeCorrecto ()
{
	NS_LOG_INFO("Rx = " << m_numPaqRx << " y Tx = " << m_paquetesTx);
	double porcentaje = ((double)m_numPaqRx / ((double) m_paquetesTx))*100;
	
	if(porcentaje>100)
		porcentaje = 100;
	
	return porcentaje;
}

//Funcion que devuelve el retardo medio calculado a lo largo de la simulacion
double Observador::GetRetardoMedio ()
{	
	return m_retardos.Mean();
}

//Funcion que devuelve el valor medio de la variacion de retardo calculada a lo largo de la simulacion
double Observador::GetVariacionMedia ()
{
	return m_variacion_retardos.Mean();
}