#ifndef CHATMESSAGE_H
#define CHATMESSAGE_H

#include "ui_ChatMessage.h"
#include "MacroDefine.h"
#include "ProcessChatMessage.h"
#include "sqlPlugin.h"
#include "AbstractWidget.h"
#include <QToolButton>


/// \brief 消息内容结构体
struct Message_Content {
	Message_Content():type(Content_Type::text),is_Self(true) {}
	QString strContent;  ///< 消息体
    /// \brief 消息类型定义
	enum class Content_Type {
		file,  ///< 文件
		image, ///< 图片
		text   ///< 文本
	};
	Content_Type type; ///< 消息类型
	bool is_Self;  ///< 是否属于自己发送的消息
};

using namespace sqlPlugin;

/// \brief 聊天界面功能定义
class ChatMessage : public AbstractWidget
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
	void InitChatMessage(const DataStructDefine& targetMessage);
	
	/// \brief 初始化聊天记录UI
	/// \param[in] targetMessage 目标聊天消息结构
	void InitMessageUI(const QMap<quint64, QList<Message_Content>>& targetContent);

	/// \brief 添加聊天消息
	/// \param[in] strMsg 消息内容
	void SetAddMessage(QString strMsg);

	/// \brief 添加聊天对象
	/// \param[in] pToolTgt 对方信息
	void SetAddChatTgt(QToolButton* pToolTgt);

	/// \brief 接收消息
	void OnMessage();

	/// \brief 保存聊天记录
	void SaveChatRecord();

	/// \brief 绘制圆形图标头像
	/// \param[in] src 原图形
	/// \param[in] radius 圆角
	/// \retval 返回绘制好的
	QPixmap PixmapToRound(const QPixmap &src, int radius);
private slots:
	
    /// \brief 发送网络消息 
    void SendTextContent(); 

	/// \brief 切换好友聊天
	/// \param[in] current 当前的
	/// \param[in] previous 之前的
	void SwitchFriend(QListWidgetItem *current, QListWidgetItem *previous);

private:
	Ui::ChatMessage ui;		///< 界面对象
	ProcessChatMessage* m_ProMsg;   ///< 处理网络任务 
};

/// \brief 气泡界面定义
class CustomMessageWidget : public QWidget {
public:
    /// \brief 构造函数
    /// \param[in] parent 父窗口
	CustomMessageWidget(QWidget* parent = 0);
	~CustomMessageWidget();
  
    /// \brief 设置消息文本内容
    /// \param[in] strContent 消息
    /// \param[in] isSelf 是否是自己的消息
	void SetText(const QString& strContent, bool isSelf);
    
    /// \brief 计算坐标，绘制聊天气泡
    /// \paran[in] event 系统参数
	void paintEvent(QPaintEvent* event);

	QString m_strContent;           ///< 消息体
	QTextEdit* m_pMessageContent;    ///< 气泡
    QTableWidget* m_pTargetTab;      ///< 气泡所处的表格
};

#endif // CHATMESSAGE_H
