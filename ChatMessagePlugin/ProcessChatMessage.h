#ifndef  __PROCESSCHATMESSAGE__
#define  __PROCESSCHATMESSAGE__
#include "AbstractNetWork.h"

class ProcessChatMessage : public AbstractNetWork
{
public:
	ProcessChatMessage(AbstractNetWork::ProtoType Type, QHostAddress addr, int port, QObject* parent = 0);
	~ProcessChatMessage();
	int RecvMessage();
	void AnalysisProtocol(QByteArray& protocol);
};

#endif
