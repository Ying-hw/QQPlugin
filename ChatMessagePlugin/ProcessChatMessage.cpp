#include "stdafx.h"
#include "NetProtocConfig.pb.h"
#include "ProcessChatMessage.h"
#include "ChatMessage.h"

extern ChatMessage* g_pChatMessage;

ProcessChatMessage::ProcessChatMessage(AbstractNetWork::ProtoType Type,	QString addr, int port, QObject* parent /*= 0*/) : AbstractNetWork(Type, addr, port, parent)
{

}

ProcessChatMessage::~ProcessChatMessage()
{

}

int ProcessChatMessage::RecvMsg()
{
	int size = 0;
	QTcpSocket* tcp = (QTcpSocket*)ReturnCurrentTargetSocket();
	if (tcp->bytesAvailable()) {
		QByteArray buf;
		buf.resize(tcp->bytesAvailable());
		size = tcp->read(buf.data(), buf.size());
		m_CurrentReadSize = size;
		AnalysisProtocol(buf);  //protobuf解析协议，把解析出来的协议内容显示到界面中
	}
	return size;
}

void ProcessChatMessage::AnalysisProtocol(QByteArray& protoArray)
{
	protocol proto;
	QString strTargetNumber;
	if (proto.ParseFromString(protoArray.toStdString())) {
		if (!m_mapChatRecord.contains(proto.myselfnum())) {
			m_mapChatRecord[proto.myselfnum()] = proto;
		}
		else {
			std::string strRecord = protoArray.toStdString();
			m_mapChatRecord[proto.myselfnum()].AppendPartialToString(&strRecord);
		}
		switch (proto.type()) {
		case protocol_MsgType_ftp:
			break;
		case protocol_MsgType_http:
			break;
		case protocol_MsgType_smtp:
			break;
		case protocol_MsgType_tcp:
			strTargetNumber = QString::fromStdString(proto.mutable_chatcontent(0)->selfnumber());
			if (strTargetNumber == g_pChatMessage->m_strSelfNum) {
				strTargetNumber = QString::fromStdString(proto.mutable_chatcontent(0)->targetnumber());
			}
			switch (proto.chatcontent(0).type())
			{
			case ChatRecord_contenttype::ChatRecord_contenttype_file:
			{
				quint64 size = proto.chatcontent(0).head().filesize();
				QString strName = QString::fromStdString(proto.chatcontent(0).head().name());
				std::string strFielData = proto.chatcontent(0).content();
				CustomMessageWidget::FileProperty fileInfo = {size, strName, QByteArray::fromStdString(strFielData)};
				g_pChatMessage->SetAddMessage(strTargetNumber, fileInfo, proto.mutable_chatcontent(0)->time(), Message_Content::Content_Type::file);
			}
				break;
			case ChatRecord_contenttype::ChatRecord_contenttype_image:
				g_pChatMessage->SetAddMessage(strTargetNumber, QString::fromStdString(proto.chatcontent(0).content()), proto.mutable_chatcontent(0)->time(), Message_Content::Content_Type::image);
				break;
			case ChatRecord_contenttype::ChatRecord_contenttype_text:
				g_pChatMessage->SetAddMessage(strTargetNumber, QString::fromStdString(proto.chatcontent(0).content()), proto.mutable_chatcontent(0)->time(), Message_Content::Content_Type::text);
				break;
			case ChatRecord_contenttype::ChatRecord_contenttype_video:
				g_pChatMessage->StartVideoChat(strTargetNumber);
				break;
			case ChatRecord_contenttype::ChatRecord_contenttype_audio:				
				g_pChatMessage->SetAddMessage(strTargetNumber, proto.mutable_chatcontent(0)->content().c_str(), proto.mutable_chatcontent(0)->time(), Message_Content::Content_Type::voice);
				break;
			case ChatRecord_contenttype::ChatRecord_contenttype_folder:
			{
				quint64 size = proto.chatcontent(0).head().filesize();
				QString strName = QString::fromStdString(proto.chatcontent(0).head().name());
				CustomMessageWidget::FileProperty fileInfo = { size, strName };		
				g_pChatMessage->SetAddMessage(strTargetNumber, fileInfo, proto.mutable_chatcontent(0)->time(), Message_Content::Content_Type::folder);
			}
				break;
			default:
				break;
			}
			break;
		case protocol_MsgType_udp:
			switch (proto.group(0).type())
			{
			case ChatRecord_Group_contenttype::ChatRecord_Group_contenttype_file:
			{
				quint64 size = proto.chatcontent(0).head().filesize();
				QString strName = QString::fromStdString(proto.chatcontent(0).head().name());
				std::string strFielData = proto.chatcontent(0).content();
				CustomMessageWidget::FileProperty fileInfo = { size, strName, strFielData.c_str() };
				g_pChatMessage->SetAddMessage(QString::fromStdString(proto.mutable_chatcontent(0)->selfnumber()), fileInfo, proto.mutable_chatcontent(0)->time(), Message_Content::Content_Type::file);
			}
				break;
			case ChatRecord_Group_contenttype::ChatRecord_Group_contenttype_image:
				g_pChatMessage->SetAddMessage(QString::fromStdString(proto.mutable_group(0)->account()), QString::fromStdString(proto.mutable_group(0)->content()), proto.mutable_group(0)->currtime(), Message_Content::Content_Type::image);
				break;
			case ChatRecord_Group_contenttype::ChatRecord_Group_contenttype_text:
				g_pChatMessage->SetAddMessage(QString::fromStdString(proto.mutable_group(0)->account()), QString::fromStdString(proto.mutable_group(0)->content()), proto.mutable_group(0)->currtime(), Message_Content::Content_Type::text);
				break;
			case ChatRecord_contenttype::ChatRecord_contenttype_audio:
				g_pChatMessage->SetAddMessage(QString::fromStdString(proto.mutable_group(0)->account()), QString::fromStdString(proto.mutable_group(0)->content()), proto.mutable_group(0)->currtime(), Message_Content::Content_Type::voice);
				break;
			case ChatRecord_contenttype::ChatRecord_contenttype_folder:
				break;
			default:
				break;
			}
			break;
		case protocol_MsgType_stateInfor:
			g_pChatMessage->UpdateFriendState(QString::fromStdString(proto.myselfnum()), proto.state());
			break;
		default:
			break;
		}
	}
}
