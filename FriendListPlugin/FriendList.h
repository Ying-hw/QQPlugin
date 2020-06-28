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
#include "NetProtocConfig.pb.h"
#include "AbstractWidget.h"

/// \brief 自定义按钮工具
/// 主要把未读消息的画出来
class FRIENDLISTPLUGIN_EXPORT CustomToolButton : public QToolButton {
	Q_OBJECT
public:
	/// \brief 构造函数
	/// \param[in] parent 父窗口
	CustomToolButton(QWidget* parent = 0);

	/// \brief 析构函数
	~CustomToolButton();

	/// \brief 绘图事件
	/// \param[in] event 系统参数
	void paintEvent(QPaintEvent *event);

	/// \brief 设置画的文字内容
	/// \param[in] strContent 内容
	void SetPaintContent(QString strContent);

	QString& GetPaintContent();
private:
	QString m_strPaintContent; ///< 画的内容
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
	QPixmap PixmapToRound(const QPixmap &src, int radius);

	/// \brief 显示未知消息的数量
	/// \param[in] strTgtNum 目标账号
	/// \param[in] isFriend 是否是好友
	void ShowUnknownMsgCount(const QString& strTgtNum, bool isFriend);

private slots:
	/// \brief 开始聊天
	void StartChat();

	/// \brief 群组聊天
	void StartGroupChat();

	/// \brief 点击消息的槽函数
	/// 点击消息开始聊天
	void StartChatFromMessage();

	void StartGroupChatFromMessage();
private:
#ifndef CHATMESSAGE_H
	Ui::FriendList ui;
#endif
	QString* m_pUserNumber;    ///< 本人账号
	QMap<CustomToolButton *, QString> m_mapGroup;    ///< 群组映射
	QMap<CustomToolButton *, QString> m_mapFriend;   ///< 好友映射
	QMap<CustomToolButton *, QString> m_mapMesssage;  ///< 消息映射
};



#endif // FRIENDLIST_H
