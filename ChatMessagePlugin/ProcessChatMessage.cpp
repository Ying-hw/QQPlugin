#include "stdafx.h"
#include "NetProtocConfig.pb.h"
#include "ProcessChatMessage.h"
#include "ChatMessage.h"

extern ChatMessage* g_pChatMessage;

ProcessChatMessage::ProcessChatMessage(AbstractNetWork::ProtoType Type, QHostAddress addr, int port, QObject* parent /*= 0*/) : AbstractNetWork(Type, addr, port, parent)
{

}

ProcessChatMessage::~ProcessChatMessage()
{

}

int ProcessChatMessage::RecvMessage()
{
	int size = 0;
	QTcpSocket* tcp = (QTcpSocket*)ReturnCurrentTargetSocket();
	if (tcp->bytesAvailable()) {
		QByteArray buf;
		buf.resize(tcp->bytesAvailable());
		size = tcp->read(buf.data(), buf.size());
		AnalysisProtocol(buf);  //protobuf解析协议，把解析出来的协议内容显示到界面中
	}
	return size;
}

void ProcessChatMessage::AnalysisProtocol(QByteArray& proto)
{
	protocolType protocol;
	if (protocol.ParseFromString(proto.toStdString())) {
		switch (protocol.type()) {
		case protocolType_Type_ftp:
			break;
		case protocolType_Type_http:
			break;
		case protocolType_Type_smtp:
			break;
		case protocolType_Type_tcp:
			switch (protocol.chatcontent(0).type())
			{
			case ChatRecord_contenttype::ChatRecord_contenttype_file:
				g_pChatMessage->SetAddMessage(QString::fromStdString(protocol.mutable_chatcontent(0)->selfnumber()), QString::fromLocal8Bit("文件："));
				break;
			case ChatRecord_contenttype::ChatRecord_contenttype_image:
				break;
			case ChatRecord_contenttype::ChatRecord_contenttype_text:
				g_pChatMessage->SetAddMessage(QString::fromStdString(protocol.mutable_chatcontent(0)->selfnumber()), QString::fromStdString(protocol.chatcontent(0).content()));
				break;
			case ChatRecord_contenttype::ChatRecord_contenttype_video:
				g_pChatMessage->StartVideoChat(QString::fromStdString(protocol.mutable_chatcontent(0)->selfnumber()));
				break;
			default:
				break;
			}
			break;
		case protocolType_Type_udp:
			switch (protocol.group(0).type())
			{
			case ChatRecord_Group_contenttype::ChatRecord_Group_contenttype_file:
				break;
			case ChatRecord_Group_contenttype::ChatRecord_Group_contenttype_image:
				break;
			case ChatRecord_Group_contenttype::ChatRecord_Group_contenttype_text:
				g_pChatMessage->SetAddMessage(QString::fromStdString(protocol.mutable_group(0)->account()), QString::fromStdString(protocol.mutable_group(0)->content()));
				break;
			default:
				break;
			}
			break;
		}
	}
}
