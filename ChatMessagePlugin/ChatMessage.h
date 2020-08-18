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

/// \brief 消息内容结构体
struct Message_Content {
	Message_Content():type(Content_Type::text),is_Self(true) {}
	QString strContent;  ///< 消息体
    /// \brief 消息类型定义
	enum class Content_Type {
		file,  ///< 文件
		image, ///< 图片
		text,   ///< 文本
		folder,  ///< 文件夹
		voice   ///< 语音 
	};
	Content_Type type; ///< 消息类型
	bool is_Self;  ///< 是否属于自己发送的消息
};

using namespace sqlPlugin;

/// \brief 气泡界面定义
class CustomMessageWidget : public QWidget
{
	Q_OBJECT
public:
	/// \brief 内容类型定义
	enum class ContentType
	{
		file,   ///< 文件
		image,  ///< 图像
		text,   ///< 文本
		folder,  ///< 文件夹
		voice   ///< 语音
	};
	struct FileProperty {
		quint64 size;
		QString strName;
		QByteArray fileData;
	};

	/// \brief 构造函数
	/// \param[in] parent 父窗口
	CustomMessageWidget(QPixmap userImage, QWidget* parent);

	/// \brief 析构函数
	~CustomMessageWidget();

	/// \brief 设置消息文本内容
	/// \param[in] strContent 消息
	/// \param[in] isSelf 是否是自己的消息
	/// \param[in] type 消息类型
	void SetContent(const QString& strContent, bool isSelf, ContentType type);

	/// \brief 传输文件与文件夹
	/// \param[in] property 文件信息
	/// \param[in] isSelf 是否是自己的消息
	/// \param[in] type 消息类型
	void SetContent(FileProperty& property, bool isSelf, ContentType type);

	/// \brief 计算坐标，绘制聊天气泡
	/// \paran[in] event 系统参数
	void paintEvent(QPaintEvent* event);

	/// \brief 初始化文字
	/// \isSelf 是不是自己发的消息
	void InitText(bool isSelf);

	/// \brief 返回消息的类型
	/// \retval 消息类型
	ContentType GetType();
private slots:
	void SlotRecv();

public:
	ContentType m_MsgType;          ///< 消息类型
	FileProperty m_MsgProperty;     ///< 消息属性  
	QByteArray m_byteContent;         ///< 消息体
private:
	QTextEdit* m_pMessageContent;    ///< 气泡
	QTableWidget* m_pTargetTab;      ///< 气泡所处的表格
	QMediaPlayer m_player;  ///< 播放语音消息
	bool m_IsClicked;  ///< 语音消息
	QProgressBar m_Bar;  ///< 读取文件进度条
	QFile  m_FileTransfer;  ///< 文件传输
	QPixmap m_UserImage;  ///< 用户头像
	QSpacerItem* m_Msgitem;
};

/// \brief 聊天界面功能定义
class CHATMESSAGEPLUGIN_EXPORT ChatMessage : public AbstractWidget
{
	Q_OBJECT

public:
    /// \brief 构造函数
    /// \param[in] parent 父窗口
	ChatMessage(QWidget *parent = 0);

	/// \brief 析构
	~ChatMessage();
	
	/// \brief 初始化聊天记录数据
	/// \param[in] targetMessage 目标聊天消息结构
	/// \param[in] tab 显示聊天记录的表格
	void InitChatMessage(const DataStructDefine& targetMessage, QTableWidget* tab);
	
	/// \brief 初始化聊天记录UI
	/// \param[in] targetMessage 目标聊天消息结构
	/// \param[in] tab 显示聊天记录的表格
	/// \param[in] strNumber 聊天记录目标账号
	void InitMessageUI(const QMap<quint64, QList<Message_Content>>& targetContent, QTableWidget* tab, QString strNumber);

	/// \brief 添加聊天消息
	/// \param[in] strTgtNum 对方账号
	/// \param[in] strMsg 消息内容
	/// \param[in] time 发送的时间
	/// \param[in] msgtype 消息类型
	/// \param[in] isSelf 是否是本人发出的消息
	void SetAddMessage(const QString strTgtNum, const QString strMsg, quint64 time, Message_Content::Content_Type msgtype, bool isSelf = false);

	/// \brief 发送文件或者文件夹的时候添加此消息
	/// \param[in] strTgtNum 对方账号
	/// \param[in] content 消息结构
	/// \param[in] time 发送消息的时间
	/// \param[in] msgtype 消息类型
	/// \param[in] isSelf 是否是本人发出的消息
	void SetAddMessage(const QString strTgtNum, CustomMessageWidget::FileProperty& content, quint64 time, Message_Content::Content_Type msgtype, bool isSelf = false);

	/// \brief 添加聊天对象
	/// \param[in] pToolTgt 对方信息
	/// \param[in] strTgtNum 对方账号
	/// \param[in] strSelfNum 本人账号
	void SetAddChatTgt(CustomToolButton* pToolTgt, const QString& strTgtNum, const QString& strSelfNum);

	/// \brief 接收消息
	void OnMessage();

	/// \brief 保存聊天记录
	void SaveChatRecord();

	/// \brief 绘制圆形图标头像
	/// \param[in] src 原图形
	/// \param[in] radius 圆角
	/// \retval 返回绘制好的
	QPixmap PixmapToRound(const QPixmap &src, int radius);

	/// \brief 启动视频聊天
	/// \param[in] strNum 对方账号
	void StartVideoChat(const QString& strNum);

	/// \brief 初始化局部协议
	/// \param[in] client 客户端类型
	/// \retval 返回协议
	protocol* InitPartProtocol(protocol_ClientType client);

	/// \brief 更新好友状态
	/// \param[in] strNum 好友账号
	/// \param[in] state 状态
	void UpdateFriendState(QString strNum, StateInformation state);

	/// \brief 初始化要发送的文件头
	/// \param[out] rec 单人聊天协议
	/// \param[out] recGroup 群聊协议
	/// \param[in] isOne 是否是单人
	/// \param[in] info 文件信息
	/// \param[out] proto 协议
	void InitSendFileHead(ChatRecord* rec, ChatRecord_Group* recGroup, protocol_Chat_OneorMultiple isOne, QFileInfo& info, protocol* proto);

	/// \brief 初始化要发送的文件头
	/// \param[out] rec 单人聊天协议
	/// \param[out] recGroup 群聊协议
	/// \param[in] isOne 是否是单人
	/// \param[out] proto 协议
	void SendFile(ChatRecord* rec, ChatRecord_Group* recGroup, protocol_Chat_OneorMultiple isOne, protocol* proto);

	/// \brief 设置按钮图片
	void SetButtonIcon();

	/// \brief 通知服务器聊天功能上线
	void TellServiceChatOnline();

	/// \brief 获取目标头像
	/// \param[in] strNumber 目标账号
	/// \retval 返回头像
	QPixmap GetTargetImage(const QString& strNumber);
private slots:
	
    /// \brief 发送网络消息 
    void SlotSendTextContent(); 

	/// \brief 切换好友聊天
	/// \param[in] current 当前的
	/// \param[in] previous 之前的
	void SlotSwitchFriend(QListWidgetItem *current, QListWidgetItem *previous);

	/// \brief 视频通话
	void SlotBtnVideo();

	/// \brief 语音通话
	void SlotBtnVoiceChat();

	/// \brief 邮件
	void SlotBtnMail();

	/// \brief 表情
	void SlotBtnFace();

	/// \brief 截图
	void SlotBtnScreenshot();

	/// \brief 震动
	void SlotBtnVibration();

	/// \brief 语音
	void SlotBtnVoice();

	/// \brief 处理绑定回车键菜单
	/// \param[in] point 鼠标位置
	void SlotCustomMenu(const QPoint& point);

	/// \brief 点击绑定回车键槽函数
	/// \param[in] isClicked 点击标识
	void SlotBindEnter(bool isClicked);

	/// \brief 点击绑定回车键槽函数
	/// \param[in] isClicked 点击标识
	void SlotRemoveBindEnter(bool isClicked);

private:
	Ui::ChatMessage ui;		///< 界面对象
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
	QString m_strSelfNum;       ///< 本人账号
	ProcessChatMessage* m_ProMsg;   ///< 处理网络任务 
	QMap<QString, QTableWidget *> m_mapNumberToTable; ///< 账号映射到聊天界面表格
	QMap<CustomToolButton *, NumInfo> m_mapFriendInfo;  ///< 是单聊还是多人聊天，以及好友的状态
	QMap<CustomMessageWidget*, QString> m_MsgSourceNum;   ///< 聊天内容--好友账号，保存该聊天内容是哪个账号的
	QMap<QString, QList<quint64>> m_mapNumberToTime;  ///< 好友映射到聊天时间
	QMap<CustomToolButton*, CustomTextEdit*> m_mapFriendToTextEdit;  ///< 好友对应文字编辑框
	QMap<QString, QLabel*> m_mapFriendState; ///< 好友状态
	friend class CustomMessageWidget;
	friend class ProcessChatMessage;
};

#endif // CHATMESSAGE_H
