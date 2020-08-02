#include "stdafx.h"
#include "NetProtocConfig.pb.h"
#include "ProsessMessage.h"
#include "FriendList.h"


extern FriendList* g_FriendList;
ProsessMessage::ProsessMessage(AbstractNetWork::ProtoType Type, QHostAddress addr, int port, QObject* parent /*= 0*/) : AbstractNetWork(Type, addr, port, parent)
{

}

int ProsessMessage::RecvMessage()
{
	int size = 0;
	QTcpSocket* tcp = (QTcpSocket*)ReturnCurrentTargetSocket();
	if (tcp->bytesAvailable()) {
		QByteArray buf;
		buf.resize(tcp->bytesAvailable());
		size = tcp->read(buf.data(), buf.size());
		AnalysisProtocol(buf);
		//protobuf解析协议，把解析出来的协议内容显示到界面中
	}
	return size;
}

void ProsessMessage::AnalysisProtocol(QByteArray& proto)
{
	protocol proto_;
	if (proto_.ParseFromString(proto.toStdString())) {
		switch (proto_.type()) {
		case protocol_MsgType_ftp:
			break;
		case protocol_MsgType_http:
			break;
		case protocol_MsgType_smtp:
			break;
		case protocol_MsgType_tcp:
			if (proto_.has_addinfor()) {
				g_FriendList->RecvFriendApply(proto_.mutable_addinfor());
				return;
			}
			switch (proto_.chatcontent(0).type())
			{
			case ChatRecord_contenttype::ChatRecord_contenttype_file:
				break;
			case ChatRecord_contenttype::ChatRecord_contenttype_image:
				break;
			case ChatRecord_contenttype::ChatRecord_contenttype_text:
				g_FriendList->ShowUnknownMsgCount(QString::fromStdString(proto_.mutable_chatcontent(0)->targetnumber()), true);
				break;
			case ChatRecord_contenttype::ChatRecord_contenttype_video:

				break;
			default:
				break;
			}
			break;
		case protocol_MsgType_udp:
			switch (proto_.group(0).type())
			{
			case ChatRecord_Group_contenttype::ChatRecord_Group_contenttype_file:
				break;
			case ChatRecord_Group_contenttype::ChatRecord_Group_contenttype_image:
				break;
			case ChatRecord_Group_contenttype::ChatRecord_Group_contenttype_text:
				g_FriendList->ShowUnknownMsgCount(QString::fromStdString(proto_.mutable_group(0)->account()), false);
				break;
			default:
				break;
			}
			break;
		case protocol_MsgType_state:
			g_FriendList->UpdateFriendState(QString::fromStdString(proto_.myselfnum()), proto_.currstate());
		default:
			break;
		}
	}
}

ProsessMessage::~ProsessMessage()
{
	deleteLater();
}
