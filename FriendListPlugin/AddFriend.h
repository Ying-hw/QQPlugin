#ifndef       __ADDFRIEND__
#define       __ADDFRIEND__


#include "AbstractWidget.h"
#include "ui_AddFriend.h"
#include "ApplyJoinFriend.h"

/// \brief 此类提供了添加好友和添加群组的功能
class AddFriend : public AbstractWidget
{
	Q_OBJECT

public:
	/// \brief 构造函数
	/// \param[in] isAddFriend 是否为添加好友，否则为添加群组
	/// \param[in] parent 父窗口
	AddFriend(bool isAddFriend, AbstractWidget *parent = Q_NULLPTR);

	/// \brief 析构函数
	~AddFriend();

private slots:

	/// \brief 查找好友
	void SlotFindFriend();

	/// \brief 查找群组
	void SlotFineGroup();

	/// \brief 开始查找
	void SlotStartFind();

	/// \brief 点击好友开始添加
	/// \param[in] row 行
	/// \param[in] col 列
	void SlotClickFriend(int row,int col);

	/// \brief 查找群组的类型
	void SlotGroupType();

	/// \brief 更改选择条件
	/// \param[in] state 状态
	void ChangedSelectCondition(int state);

private:
	Ui::AddFriend ui;
	bool m_isAddFriend;  ///< 添加好友标识
};

/// \brief 自定义添加信息界面，在表格中显示目标的信息
class CustomAddInformationWidget : public AbstractWidget {
	Q_OBJECT
public:
	/// \brief 构造函数
	/// \param[in] strName 用户名称
	/// \param[in] strAttr 用户属性
	/// \param[in] arrayImage 图像数据
	/// \param[in] parent 父窗口
	CustomAddInformationWidget(QString strName, QString strAttr, QByteArray& arrayImage, bool isFriend, AbstractWidget* parent = 0);

	/// \brief 构造函数
	/// \param[in] parent 父窗口
	CustomAddInformationWidget(AbstractWidget* parent = 0);

	/// \brief 析构函数
	~CustomAddInformationWidget();

	/// \brief 绘制文字
	/// \param[in] event 系统参数
	void paintEvent(QPaintEvent *event);

	/// \brief 设置用户属性
	/// \param[in] strName 用户名称
	/// \param[in] strAttr 用户属性
	/// \param[in] arrayImage 图像数据
	void SetAttribute(QString strName, QString strAttr, QByteArray& arrayImage);

private slots:

	/// \brief 显示目标信息
	void TgtInfor();

	/// \brief 确定添加
	void Add();
private:
	QPushButton* m_BtnAddButton;
	QToolButton* m_BtnTgtInfo;
	QString m_strUserName;
	QString m_ExtraParam;
	QPixmap m_Pixmap;
	ApplyJoinFriend* m_Apply;
	bool m_IsFriend;
	AbstractWidget* m_UserWidget;
};


#endif
