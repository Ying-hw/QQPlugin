#ifndef FRIENDLIST_H
#define FRIENDLIST_H

#ifndef CHATMESSAGE_H
#include "ui_FriendList.h"
#endif
#include "MacroDefine.h"
#include "friendlistplugin_global.h"
#include "AbstractNetWork.h"
#include "NetProtocConfig.pb.h"
#include "SqlStatementDefine.h"
#include "AbstractWidget.h"
#include "ProsessMessage.h"


/// \brief 自定义按钮工具
/// 主要把未读消息的画出来
class FRIENDLISTPLUGIN_EXPORT CustomToolButton : public QToolButton 
{
	Q_OBJECT
public:
	/// \brief 构造函数
	/// \param[in] parent 父窗口
	CustomToolButton(QWidget* parent = 0);

	/// \brief 析构函数
	~CustomToolButton();

	CustomToolButton(const CustomToolButton* button);

	const CustomToolButton& operator=(const CustomToolButton* button);

	/// \brief 绘图事件
	/// \param[in] event 系统参数
	void paintEvent(QPaintEvent *event);

	/// \brief 设置画的文字内容
	/// \param[in] strContent 内容
	void SetPaintContent(QString strContent);

	/// \brief 获取绘图的内容
	QString& GetPaintContent();
private:
	QString m_strPaintContent; ///< 画的内容
};

/// \brief 好友申请验证
class CustomAddFriendMessageHint : public AbstractWidget
{
	Q_OBJECT
public:
	/// \brief 构造函数         
	/// \param[in] parent 父窗口
	CustomAddFriendMessageHint(AbstractWidget* parent = 0);

	/// \brief 析构函数
	~CustomAddFriendMessageHint();

	/// \brief 设置布局
	/// \param[in] infor 好友信息
	void SetLayout(AddInformation* infor);

private slots:
	/// \brief 同意添加
	void ConsentApply();

	/// \brief 显示好友信息
	void UserInfor();
private:
	AbstractWidget* m_UserWidget;  ///< 好友信息
	bool m_IsAddFriend; ///< 添加群还是好友
	bool m_Gender; ///< 性别 
	AddInformation m_infor;   ///< 添加信息 
};

class FRIENDLISTPLUGIN_EXPORT FriendList : public AbstractWidget
{
	Q_OBJECT

public:
	FriendList(QWidget *parent = 0);
	~FriendList();
	/// \brief 当登录的时候恢复聊天内容
	void RecoveryChatRecord();

	/// \brief 设置好友列表界面
	/// \param[in] strNum 好友账号
	/// \param[in] strName 好友的用户名称
	/// \param[in] byteImage 好友的头像
	/// \param[in] isFriend 单聊
	void SetMessage_ListUi(const QString& strNum, const QString& strName, const QByteArray& byteImage, bool isFriend);

	/// \brief 初始化好友列表
	void InitFriendList();

	/// \brief 初始化群组
	void InitGroupList();

	/// \brief 初始化QQ空间
	void InitQQSpaceList();

	/// \brief 绘制圆形图标头像
	/// \param[in] src 原图形
	/// \param[in] radius 圆角
	/// \retval 返回绘制好的
	static QPixmap PixmapToRound(const QPixmap &src, int radius);

	/// \brief 显示未知消息的数量
	/// \param[in] strTgtNum 目标账号
	/// \param[in] isFriend 是否是好友
	void ShowUnknownMsgCount(const QString& strTgtNum, bool isFriend);

	/// \brief 启动视频聊天
	/// \param[in] strNum 对方账号
	void StartVideoChat(const QString& strNum);

	/// \brief 接受添加好友的请求
	/// \param[in] infor 对方信息
	void RecvFriendApply(AddInformation* infor);

	/// \brief 调整菜单位置
	/// \param[in] event 系统参数
	void paintEvent(QPaintEvent *event);

	/// \brief 更新好友的状态
	/// \param[in] strNum 好友账号
	/// \param[in] state 状态信息
	void UpdateFriendState(const QString strNum, StateInformation state);

	/// \brief 彩色头像转换灰度图像
	/// \param[in] icon 图像源
	/// \param[in] size 大小
	/// \retval 返回转换后的灰度头像
	static QImage convertImage(QIcon iconSource, QSize size);

	/// \brief 保存聊天记录
	/// \param[in] proto 协议
	void SaveChatRecord(protocol& proto);

	/// \brief 获取当前状态
	/// \param[in] proto 原协议
	/// \retval 返回当前状态
	static void setCurrentState(protocol& proto);

	static QString* m_pUserNumber;    ///< 本人账号
	static ProsessMessage* m_NetWorkProsess;  ///< 处理网络消息

signals:
	/// \brief 使用信号初始化可以帮助界面增加显示速度，不阻塞
	void InitAllMember();

private slots:

	/// \brief 初始化成员
	void Initialization();

	/// \brief 开始聊天
	void SlotStartChat();

	/// \brief 群组聊天
	void SlotStartGroupChat();

	/// \brief 点击消息的槽函数
	/// 点击消息开始聊天
	void SlotStartChatFromMessage();

	/// \brief 从消息体获取信息并且开始群组聊天
	void SlotStartGroupChatFromMessage();

	/// \brief 添加朋友或者群组
	/// \param[in] isClicked 点击标识
	void SlotAdd(bool isClicked = false);

	/// \brief 切换好友列表消息列表空间
	void SwitchFriMsgSpace();

	/// \brief 状态改变（在线，离线，隐身，勿扰）
	/// \param[in] strCurrentText 当前状态
	void SlotChangedState(const QString &strCurrentText);

private:
#ifndef CHATMESSAGE_H
	Ui::FriendList ui;
#endif
	QMap<CustomToolButton *, QString> m_mapGroup;    ///< 群组映射
	QMap<CustomToolButton *, QString> m_mapFriend;   ///< 好友映射
	QMap<CustomToolButton *, QString> m_mapMesssage;  ///< 消息映射
	QMenu* m_pSystemMenu;  ///< 系统菜单
	QTreeWidgetItem* m_pFriendTree; ///< 好友列表
	QTreeWidgetItem* m_pGroupTree;  ///< 群组列表
};


#endif // FRIENDLIST_H
