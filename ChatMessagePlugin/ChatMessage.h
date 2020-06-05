#ifndef CHATMESSAGE_H
#define CHATMESSAGE_H

#include <QWidget>
#include "ui_ChatMessage.h"
#include "MacroDefine.h"
#include "sqlPlugin.h"

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
class ChatMessage : public QWidget
{
	Q_OBJECT

public:
    /// \brief 构造函数
    /// \param[in] parent 父窗口
	ChatMessage(QWidget *parent = 0);
	~ChatMessage();
	
	/// \brief 初始化聊天记录数据
	/// \param[in] targetMessage 目标聊天消息结构
	void InitChatMessage(const DataStructDefine& targetMessage);
	
	/// \brief 初始化聊天记录UI
	/// \param[in] targetMessage 目标聊天消息结构
	void InitMessageUI(const QMap<quint64, QList<Message_Content>>& targetContent);
private slots:
	
    /// \brief 发送网络消息 
    void SendTextContent();

private:
	Ui::ChatMessage ui;
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
