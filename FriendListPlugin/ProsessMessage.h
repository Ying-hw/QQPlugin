#ifndef  PROCESSMASSAGE_H__
#define  PROCESSMASSAGE_H__
#include "AbstractNetWork.h"
class ProsessMessage : public AbstractNetWork
{
public:
	ProsessMessage(AbstractNetWork::ProtoType Type, QString addr, int port, QObject* parent = 0);
	
	~ProsessMessage();

	int RecvMsg();
	
	/// \brief ����Э��
	/// \param[in] protocol ԭЭ������
	void AnalysisProtocol(QByteArray& protocol);

};

#endif

