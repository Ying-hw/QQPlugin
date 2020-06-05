#include "stdafx.h"
#include "ProsessMessage.h"
#include <QAbstractSocket>

ProsessMessage::ProsessMessage(QObject* parent /*= 0*/) : 
	NetWork(QAbstractSocket::SocketType::TcpSocket, parent)
{

}
