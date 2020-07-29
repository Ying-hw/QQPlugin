#ifndef       __ADDFRIEND__
#define       __ADDFRIEND__
#include "AbstractWidget.h"
#include "ui_AddFriend.h"
#include "ApplyJoinFriend.h"

/// \brief 用户信息定义
struct TargetInfor
{
	quint32 m_Age;				///< 年龄
	quint32 memberCount;        ///< 群成员数量
	QString m_strGroupType;     ///< 群组类型
	QString m_Occupational;		///< 职业
	QString m_Address;			///< 地址
	QString m_strNumber;		///< 账号
	QString m_strSchool;        ///< 毕业院校
	QString m_strBirthPlace;	///< 故乡
	QString m_strName;          ///< 用户名称
	QString m_Gender;           ///< 性别
	QPixmap pix;                ///< 头像
	bool m_IsFriend;            ///< 用户或者群

};

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
	/// \param[in] infor 目标信息
	/// \param[in] parent 父窗口
	CustomAddInformationWidget(TargetInfor& infor, AbstractWidget* parent = 0);

	/// \brief 构造函数
	/// \param[in] parent 父窗口
	CustomAddInformationWidget(AbstractWidget* parent = 0);

	/// \brief 析构函数
	~CustomAddInformationWidget();

	/// \brief 绘制文字
	/// \param[in] event 系统参数
	void paintEvent(QPaintEvent *event);

private slots:

	/// \brief 显示目标信息
	void TgtInfor();

	/// \brief 确定添加
	void Add();
private:
	QPushButton* m_BtnAddButton;
	QToolButton* m_BtnTgtInfo;
	ApplyJoinFriend* m_Apply;
	TargetInfor m_Infor;
	AbstractWidget* m_UserWidget;
};
#endif
