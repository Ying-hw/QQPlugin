#ifndef CHATMESSAGE_H
#define CHATMESSAGE_H

#include "ui_ChatMessage.h"
#include "NetProtocConfig.pb.h"
#include "MacroDefine.h"
#include "ProcessChatMessage.h"
#include "sqlPlugin.h"
#include "AbstractWidget.h"
#include "../FriendListPlugin/FriendList.h"
#include "ChatMessage_global.h"

/// \brief ��Ϣ���ݽṹ��
struct Message_Content {
	Message_Content():type(Content_Type::text),is_Self(true) {}
	QString strContent;  ///< ��Ϣ��
    /// \brief ��Ϣ���Ͷ���
	enum class Content_Type {
		file,  ///< �ļ�
		image, ///< ͼƬ
		text,   ///< �ı�
		folder,  ///< �ļ���
		voice   ///< ���� 
	};
	Content_Type type; ///< ��Ϣ����
	bool is_Self;  ///< �Ƿ������Լ����͵���Ϣ
};

using namespace sqlPlugin;

/// \brief ���ݽ��涨��
class CustomMessageWidget : public QWidget
{
	Q_OBJECT
public:
	/// \brief �������Ͷ���
	enum class ContentType
	{
		file,   ///< �ļ�
		image,  ///< ͼ��
		text,   ///< �ı�
		folder,  ///< �ļ���
		voice   ///< ����
	};
	struct FileProperty {
		quint64 size;
		QString strName;
		QByteArray fileData;
	};

	/// \brief ���캯��
	/// \param[in] parent ������
	CustomMessageWidget(QPixmap userImage, QWidget* parent);

	/// \brief ��������
	~CustomMessageWidget();

	/// \brief ������Ϣ�ı�����
	/// \param[in] strContent ��Ϣ
	/// \param[in] isSelf �Ƿ����Լ�����Ϣ
	/// \param[in] type ��Ϣ����
	void SetContent(const QString& strContent, bool isSelf, ContentType type);

	/// \brief �����ļ����ļ���
	/// \param[in] property �ļ���Ϣ
	/// \param[in] isSelf �Ƿ����Լ�����Ϣ
	/// \param[in] type ��Ϣ����
	void SetContent(FileProperty& property, bool isSelf, ContentType type);

	/// \brief �������꣬������������
	/// \paran[in] event ϵͳ����
	void paintEvent(QPaintEvent* event);

	/// \brief ��ʼ������
	/// \isSelf �ǲ����Լ�������Ϣ
	void InitText(bool isSelf);

	/// \brief ������Ϣ������
	/// \retval ��Ϣ����
	ContentType GetType();
private slots:
	void SlotRecv();

public:
	ContentType m_MsgType;          ///< ��Ϣ����
	FileProperty m_MsgProperty;     ///< ��Ϣ����  
	QByteArray m_byteContent;         ///< ��Ϣ��
private:
	QTextEdit* m_pMessageContent;    ///< ����
	QTableWidget* m_pTargetTab;      ///< ���������ı��
	QMediaPlayer m_player;  ///< ����������Ϣ
	bool m_IsClicked;  ///< ������Ϣ
	QProgressBar m_Bar;  ///< ��ȡ�ļ�������
	QFile  m_FileTransfer;  ///< �ļ�����
	QPixmap m_UserImage;  ///< �û�ͷ��
	QSpacerItem* m_Msgitem;
};

/// \brief ������湦�ܶ���
class CHATMESSAGEPLUGIN_EXPORT ChatMessage : public AbstractWidget
{
	Q_OBJECT

public:
    /// \brief ���캯��
    /// \param[in] parent ������
	ChatMessage(QWidget *parent = 0);

	/// \brief ����
	~ChatMessage();
	
	/// \brief ��ʼ�������¼����
	/// \param[in] targetMessage Ŀ��������Ϣ�ṹ
	/// \param[in] tab ��ʾ�����¼�ı��
	void InitChatMessage(const DataStructDefine& targetMessage, QTableWidget* tab);
	
	/// \brief ��ʼ�������¼UI
	/// \param[in] targetMessage Ŀ��������Ϣ�ṹ
	/// \param[in] tab ��ʾ�����¼�ı��
	/// \param[in] strNumber �����¼Ŀ���˺�
	void InitMessageUI(const QMap<quint64, QList<Message_Content>>& targetContent, QTableWidget* tab, QString strNumber);

	/// \brief ���������Ϣ
	/// \param[in] strTgtNum �Է��˺�
	/// \param[in] strMsg ��Ϣ����
	/// \param[in] time ���͵�ʱ��
	/// \param[in] msgtype ��Ϣ����
	/// \param[in] isSelf �Ƿ��Ǳ��˷�������Ϣ
	void SetAddMessage(const QString strTgtNum, const QString strMsg, quint64 time, Message_Content::Content_Type msgtype, bool isSelf = false);

	/// \brief �����ļ������ļ��е�ʱ����Ӵ���Ϣ
	/// \param[in] strTgtNum �Է��˺�
	/// \param[in] content ��Ϣ�ṹ
	/// \param[in] time ������Ϣ��ʱ��
	/// \param[in] msgtype ��Ϣ����
	/// \param[in] isSelf �Ƿ��Ǳ��˷�������Ϣ
	void SetAddMessage(const QString strTgtNum, CustomMessageWidget::FileProperty& content, quint64 time, Message_Content::Content_Type msgtype, bool isSelf = false);

	/// \brief ����������
	/// \param[in] pToolTgt �Է���Ϣ
	/// \param[in] strTgtNum �Է��˺�
	/// \param[in] strSelfNum �����˺�
	void SetAddChatTgt(CustomToolButton* pToolTgt, const QString& strTgtNum, const QString& strSelfNum);

	/// \brief ������Ϣ
	void OnMessage();

	/// \brief ���������¼
	void SaveChatRecord();

	/// \brief ����Բ��ͼ��ͷ��
	/// \param[in] src ԭͼ��
	/// \param[in] radius Բ��
	/// \retval ���ػ��ƺõ�
	QPixmap PixmapToRound(const QPixmap &src, int radius);

	/// \brief ������Ƶ����
	/// \param[in] strNum �Է��˺�
	void StartVideoChat(const QString& strNum);

	/// \brief ��ʼ���ֲ�Э��
	/// \param[in] client �ͻ�������
	/// \retval ����Э��
	protocol* InitPartProtocol(protocol_ClientType client);

	/// \brief ���º���״̬
	/// \param[in] strNum �����˺�
	/// \param[in] state ״̬
	void UpdateFriendState(QString strNum, StateInformation state);

	/// \brief ��ʼ��Ҫ���͵��ļ�ͷ
	/// \param[out] rec ��������Э��
	/// \param[out] recGroup Ⱥ��Э��
	/// \param[in] isOne �Ƿ��ǵ���
	/// \param[in] info �ļ���Ϣ
	/// \param[out] proto Э��
	void InitSendFileHead(ChatRecord* rec, ChatRecord_Group* recGroup, protocol_Chat_OneorMultiple isOne, QFileInfo& info, protocol* proto);

	/// \brief ��ʼ��Ҫ���͵��ļ�ͷ
	/// \param[out] rec ��������Э��
	/// \param[out] recGroup Ⱥ��Э��
	/// \param[in] isOne �Ƿ��ǵ���
	/// \param[out] proto Э��
	void SendFile(ChatRecord* rec, ChatRecord_Group* recGroup, protocol_Chat_OneorMultiple isOne, protocol* proto);

	/// \brief ���ð�ťͼƬ
	void SetButtonIcon();

	/// \brief ֪ͨ���������칦������
	void TellServiceChatOnline();

	/// \brief ��ȡĿ��ͷ��
	/// \param[in] strNumber Ŀ���˺�
	/// \retval ����ͷ��
	QPixmap GetTargetImage(const QString& strNumber);
private slots:
	
    /// \brief ����������Ϣ 
    void SlotSendTextContent(); 

	/// \brief �л���������
	/// \param[in] current ��ǰ��
	/// \param[in] previous ֮ǰ��
	void SlotSwitchFriend(QListWidgetItem *current, QListWidgetItem *previous);

	/// \brief ��Ƶͨ��
	void SlotBtnVideo();

	/// \brief ����ͨ��
	void SlotBtnVoiceChat();

	/// \brief �ʼ�
	void SlotBtnMail();

	/// \brief ����
	void SlotBtnFace();

	/// \brief ��ͼ
	void SlotBtnScreenshot();

	/// \brief ��
	void SlotBtnVibration();

	/// \brief ����
	void SlotBtnVoice();

	/// \brief ����󶨻س����˵�
	/// \param[in] point ���λ��
	void SlotCustomMenu(const QPoint& point);

	/// \brief ����󶨻س����ۺ���
	/// \param[in] isClicked �����ʶ
	void SlotBindEnter(bool isClicked);

	/// \brief ����󶨻س����ۺ���
	/// \param[in] isClicked �����ʶ
	void SlotRemoveBindEnter(bool isClicked);

private:
	Ui::ChatMessage ui;		///< �������
	struct NumInfo
	{
		NumInfo() : m_unknowMessage(0), m_IsFriend(true), m_isCurrent(false) {}
		NumInfo(bool Friend_Group, bool isCurrent, QString& strNum, quint32 unknowMessage)
			: m_IsFriend(Friend_Group), m_isCurrent(isCurrent), m_strNum(strNum), m_unknowMessage(unknowMessage) {}
		bool m_IsFriend;
		bool m_isCurrent;
		QString m_strNum;
		quint32 m_unknowMessage;
	};
	QString m_strSelfNum;       ///< �����˺�
	ProcessChatMessage* m_ProMsg;   ///< ������������ 
	QMap<QString, QTableWidget *> m_mapNumberToTable; ///< �˺�ӳ�䵽���������
	QMap<CustomToolButton *, NumInfo> m_mapFriendInfo;  ///< �ǵ��Ļ��Ƕ������죬�Լ����ѵ�״̬
	QMap<CustomMessageWidget*, QString> m_MsgSourceNum;   ///< ��������--�����˺ţ�����������������ĸ��˺ŵ�
	QMap<QString, QList<quint64>> m_mapNumberToTime;  ///< ����ӳ�䵽����ʱ��
	QMap<CustomToolButton*, CustomTextEdit*> m_mapFriendToTextEdit;  ///< ���Ѷ�Ӧ���ֱ༭��
	QMap<QString, QLabel*> m_mapFriendState; ///< ����״̬
	friend class CustomMessageWidget;
	friend class ProcessChatMessage;
};

#endif // CHATMESSAGE_H
