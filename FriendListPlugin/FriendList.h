#ifndef FRIENDLIST_H
#define FRIENDLIST_H

#include "ui_FriendList.h"
#include "MacroDefine.h"
#include "friendlistplugin_global.h"
#include "NetWork.h"
#include "NetProtocConfig.pb.h"
#include "SqlStatementDefine.h"
#include "NetProtocConfig.pb.h"

class FRIENDLISTPLUGIN_EXPORT FriendList : public QWidget
{
	Q_OBJECT

public:
	FriendList(QWidget *parent = 0);
	~FriendList();
	void RecoveryChatRecord();
	void SetMessage_ListUi(const QString& strName, const QByteArray& byteImage);
	void paintEvent(QPaintEvent *event);
	void InitFriendList();
	void InitGroupList();
	void InitQQSpaceList();
	QPixmap PixmapToRound(const QPixmap &src, int radius);
private slots:
	void StartChat();
	void StartGroupChat();
private:
	Ui::FriendList ui;
	QString* m_pUserNumber;
	QMap<QToolButton *, QString> m_mapGroup;
	QMap<QToolButton *, QString> m_mapFriend;
};

#endif // FRIENDLIST_H
