#ifndef  __PROCESSCHATMESSAGE__
#define  __PROCESSCHATMESSAGE__
#include "AbstractNetWork.h"

class ProcessChatMessage : public AbstractNetWork
{
public:
	ProcessChatMessage(AbstractNetWork::ProtoType Type, QHostAddress addr, int port, QObject* parent = 0);
	~ProcessChatMessage();
	int RecvMsg();
	void AnalysisProtocol(QByteArray& protocol);
private:
	qint64 m_CurrentReadSize;
	friend class CustomMessageWidget;
};

#endif
