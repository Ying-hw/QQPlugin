#ifndef FRIENDLIST_H
#define FRIENDLIST_H

#include "ui_FriendList.h"
#include "MacroDefine.h"
#include "friendlistplugin_global.h"
#include "AbstractNetWork.h"
#include "NetProtocConfig.pb.h"
#include "SqlStatementDefine.h"
#include "NetProtocConfig.pb.h"
#include "AbstractWidget.h"

class FRIENDLISTPLUGIN_EXPORT FriendList : public AbstractWidget
{
	Q_OBJECT

public:
	FriendList(QWidget *parent = 0);
	~FriendList();
	/// \brief 当登录的时候恢复聊天内容
	void RecoveryChatRecord();

	/// \brief 设置好友列表界面
	/// \param[in] strName 好友的用户名称
	/// \param[in] byteImage 好友的头像
	void SetMessage_ListUi(const QString& strName, const QByteArray& byteImage);

	/// \brief 绘图事件
	/// 计算未读消息的数量，并画在消息消息界面上
	/// \param[in] event 系统参数
	void paintEvent(QPaintEvent *event);

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
private slots:
	/// \brief 开始聊天
	void StartChat();

	/// \brief 群组聊天
	void StartGroupChat();
private:
	Ui::FriendList ui;
	QString* m_pUserNumber;    ///< 本人账号
	QMap<QToolButton *, QString> m_mapGroup;    ///< 群组映射
	QMap<QToolButton *, QString> m_mapFriend;   ///< 好友映射
};

#endif // FRIENDLIST_H
