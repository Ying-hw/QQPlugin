#ifndef  __PROCESSCHATMESSAGE__
#define  __PROCESSCHATMESSAGE__
#include "AbstractNetWork.h"

class ProcessChatMessage : public AbstractNetWork
{
public:
	ProcessChatMessage(AbstractNetWork::ProtoType Type, QString addr, int port, QObject* parent = 0);
	~ProcessChatMessage();
	int RecvMsg();
	void AnalysisProtocol(QByteArray& protocol);
	QMap<std::string, protocol> m_mapChatRecord;
private:
	qint64 m_CurrentReadSize;
	friend class CustomMessageWidget;
};

#endif
