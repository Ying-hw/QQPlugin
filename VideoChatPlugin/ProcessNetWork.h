#ifndef __PROCESSNETWORK__
#define __PROCESSNETWORK__
#include "AbstractNetWork.h"

class ProcessNetWork : public AbstractNetWork
{
public:
	ProcessNetWork(AbstractNetWork::ProtoType Type, QHostAddress addr, int port, QObject* parent = 0);
	~ProcessNetWork();
	int RecvMessage();
	void AnalysisProtocol(QByteArray& protocol);
};

#endif

