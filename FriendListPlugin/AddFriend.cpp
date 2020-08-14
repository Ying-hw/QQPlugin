#include "stdafx.h"
#include "AddFriend.h"
#include "MacroDefine.h"
#include "SqlStatementDefine.h"
#include "ApplyJoinFriend.h"
#include "FriendList.h"

#define IMAGE_SIZE  60
#define COLUMN_COUNT 5

AddFriend::AddFriend(bool isFriend, AbstractWidget *parent) : AbstractWidget(parent), m_isAddFriend(isFriend)
{
	ui.setupUi(this);
	m_isAddFriend ? SlotFindFriend() : SlotFineGroup();
	connect(ui.BtnFindFriend, SIGNAL(clicked()), this, SLOT(SlotFindFriend()));
	connect(ui.BtnFindGroup, SIGNAL(clicked()), this, SLOT(SlotFineGroup()));
	connect(ui.BtnFind, SIGNAL(clicked()), this, SLOT(SlotStartFind()));
	connect(ui.tabFriendRecord, SIGNAL(cellClicked(int, int)), this, SLOT(SlotClickFriend(int,int)));
	connect(ui.BtnGame, SIGNAL(clicked()), this, SLOT(SlotGroupType()));
	connect(ui.BtnIndustry, SIGNAL(clicked()), this, SLOT(SlotGroupType()));
	connect(ui.BtnLearning, SIGNAL(clicked()), this, SLOT(SlotGroupType()));
	connect(ui.BtnLife, SIGNAL(clicked()), this, SLOT(SlotGroupType()));
	connect(ui.BtnRecreation, SIGNAL(clicked()), this, SLOT(SlotGroupType()));
	connect(ui.ChecInputFrame, SIGNAL(stateChanged(int)), this, SLOT(ChangedSelectCondition(int)));
	connect(ui.ChecCondition, SIGNAL(stateChanged(int)), this, SLOT(ChangedSelectCondition(int)));
	ui.tabFriendRecord->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.tabFriendRecord->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
	ui.tabFriendRecord->horizontalHeader()->setVisible(false);
	ui.tabFriendRecord->verticalHeader()->setVisible(false);
	ui.ComAge->setEnabled(!ui.ChecInputFrame->isChecked());
	ui.ComGender->setEnabled(!ui.ChecInputFrame->isChecked());
	ui.ComHometown->setEnabled(!ui.ChecInputFrame->isChecked());
	ui.ComOccupation->setEnabled(!ui.ChecInputFrame->isChecked());
	ui.tabFriendRecord->setColumnCount(5);
	ui.tabFriendRecord->setObjectName("tabFriendRecord");
}

AddFriend::~AddFriend()
{
}

void AddFriend::SlotFindFriend()
{
	m_isAddFriend = true;
	ui.BtnFindFriend->setChecked(true);
	ui.ComAge->show();
	ui.ComGender->show();
	ui.ComHometown->show();
	ui.ComOccupation->show();
	ui.widget_5->hide();
	ui.ChecCondition->show();
	ui.ChecInputFrame->show();
}

void AddFriend::SlotFineGroup()
{
	m_isAddFriend = false;
	ui.BtnFindGroup->setChecked(true);
	ui.ComAge->hide();
	ui.ComGender->hide();
	ui.ComHometown->hide();
	ui.ComOccupation->hide();
	ui.widget_5->show();
	ui.ChecCondition->hide();
	ui.ChecInputFrame->hide();
}


void AddFriend::ChangedSelectCondition(int state)
{
	QCheckBox * TgtCheck = qobject_cast<QCheckBox*>(sender());
	if (TgtCheck == ui.ChecCondition) {
		ui.EditInput->setEnabled(state == Qt::CheckState::Unchecked);
		ui.ComAge->setEnabled(state == Qt::CheckState::Checked);
		ui.ComGender->setEnabled(state == Qt::CheckState::Checked);
		ui.ComHometown->setEnabled(state == Qt::CheckState::Checked);
		ui.ComOccupation->setEnabled(state == Qt::CheckState::Checked);
	}
	if (TgtCheck == ui.ChecInputFrame) {
		ui.EditInput->setEnabled(state == Qt::CheckState::Checked);
		ui.ComAge->setEnabled(state == Qt::CheckState::Unchecked);
		ui.ComGender->setEnabled(state == Qt::CheckState::Unchecked);
		ui.ComHometown->setEnabled(state == Qt::CheckState::Unchecked);
		ui.ComOccupation->setEnabled(state == Qt::CheckState::Unchecked);
	}
}

void AddFriend::SlotStartFind()
{
	ui.tabFriendRecord->clearContents();
	QString strSql = QString(SELECT_FIND_USER).arg(m_isAddFriend ? "user_account" : "groupss");
	if (m_isAddFriend) 
		if (ui.ChecCondition->isChecked()) {
			QStringList strAgeList = ui.ComAge->currentText().split("-", QString::SkipEmptyParts);
			if (!strAgeList.isEmpty())
				strSql += QString(" AGE BETWEEN %1 AND %2").arg(strAgeList.at(0).toInt()).arg(strAgeList.at(1).toInt());
			if (!ui.ComGender->currentText().contains(QString::fromLocal8Bit("不限")))
				strSql += QString(" AND GENDER = '%1'").arg(ui.ComGender->currentIndex() - 1);
			if (!ui.ComHometown->currentText().contains(QString::fromLocal8Bit("不限")))
				strSql += QString(" AND BIRTHPLACE = '%1'").arg(ui.ComHometown->currentText());
			if (ui.ComOccupation->currentText().contains(QString::fromLocal8Bit("不限")))
				strSql += QString(" AND OCCUPATIONAL = '%1'").arg(ui.ComOccupation->currentText());
		}
		else
			strSql += QString(" USER_NUMBER = '%1' OR USER_NAME = '%1'").arg(ui.EditInput->text());
	else 
		strSql += QString(" GROUP_NAME = '%1' OR GROUP_ACCOUNT = '%1'").arg(ui.EditInput->text());
	sqlPlugin::DataStructDefine dataLib;
	sqlPlugin::DataStructDefine& data = GET_DATA(dataLib, strSql);
	if (!data.m_lstAllData.isEmpty()) {
		int count = data.m_lstAllData.size();
		ui.tabFriendRecord->setRowCount(!(count % COLUMN_COUNT) ? count / COLUMN_COUNT : count / COLUMN_COUNT + 1);
	}
	for (int i = 0; i < data.m_lstAllData.size(); i++) {
		QString strGroupName = data.m_lstAllData[i][m_isAddFriend ? "USER_NAME" : "GROUP_NAME"].toString();
		QByteArray array = data.m_lstAllData[i]["IMAGE"].toByteArray();
		QImage im;
		im.loadFromData(array);
		TargetInfor infor;
		infor.m_IsFriend = m_isAddFriend;
		if (!m_isAddFriend) {
			QString strCount = QString(SELECT_GROUP_PEOPLECOUNT).arg(data.m_lstAllData[i]["GROUP_ACCOUNT"].toString());
			sqlPlugin::DataStructDefine dataLib;
			sqlPlugin::DataStructDefine& dataCount = GET_DATA(dataLib, strCount);
			infor.memberCount = dataCount.m_lstAllData.size();
			infor.m_strNumber = data.m_lstAllData[i]["GROUP_ACCOUNT"].toString();
			infor.m_strGroupType = dataCount.m_lstAllData[0]["TYPE"].toString();
			infor.m_strName = dataCount.m_lstAllData[0]["GROUP_NAME"].toString();
			infor.pix = QPixmap::fromImage(im);	
		}
		else {
			QString strUser = QString(SELECT_USER).arg(data.m_lstAllData[i]["USER_NUMBER"].toString());
			sqlPlugin::DataStructDefine dataLib;
			sqlPlugin::DataStructDefine& UserData = GET_DATA(dataLib, strUser);
			infor.m_strName = UserData.m_lstAllData[0]["USER_NAME"].toString();
			infor.pix = QPixmap::fromImage(im);
			infor.m_Address = UserData.m_lstAllData[0]["ADDRESS"].toString();
			infor.m_strNumber = UserData.m_lstAllData[i]["USER_NUMBER"].toString();
			infor.m_strSchool = UserData.m_lstAllData[i]["SCHOOL"].toString();
			infor.m_strBirthPlace = UserData.m_lstAllData[i]["BIRTHPLACE"].toString();
			infor.m_Occupational = UserData.m_lstAllData[i]["OCCUPATIONAL"].toString();
			infor.m_Gender = UserData.m_lstAllData[i]["GENDER"].toInt() ? QString::fromLocal8Bit("女") : QString::fromLocal8Bit("男");
			infor.m_Age = UserData.m_lstAllData[i]["AGE"].toUInt();
		}
		CustomAddInformationWidget* pAddInfor = new CustomAddInformationWidget(infor, this);
		ui.tabFriendRecord->setCellWidget(i / COLUMN_COUNT, i % COLUMN_COUNT, pAddInfor);
	}
}

void AddFriend::SlotClickFriend(int row, int col)
{
	
	
}

void AddFriend::SlotGroupType()
{
	sqlPlugin::DataStructDefine dataLib, dataLib1;
	QPushButton *button = qobject_cast<QPushButton*>(sender());
	QString strSelect = QString(SELECT_GROUP_TYPE).arg(button->text());
	sqlPlugin::DataStructDefine& data = GET_DATA(dataLib, strSelect);
	if (!data.m_lstAllData.isEmpty()) {
		int count = data.m_lstAllData.size();
		ui.tabFriendRecord->setRowCount(!(count % COLUMN_COUNT) ? count / COLUMN_COUNT : count / COLUMN_COUNT + 1);
	}
	for (int i = 0;i < data.m_lstAllData.size();i++) {
		TargetInfor infor;
		QString strGroupName = data.m_lstAllData[i]["GROUP_NAME"].toString();
		QByteArray array = QString::fromUtf8(data.m_lstAllData[i]["IMAGE"].toByteArray()).toLocal8Bit();
		QString strCount = QString(SELECT_GROUP_PEOPLECOUNT).arg(data.m_lstAllData[i]["GROUP_ACCOUNT"].toString());
		sqlPlugin::DataStructDefine& dataCount = GET_DATA(dataLib1, strCount);

		QImage im;
		im.loadFromData(array);
		infor.memberCount = dataCount.m_lstAllData.size();
		infor.m_strNumber = data.m_lstAllData[i]["GROUP_ACCOUNT"].toString();
		infor.m_strGroupType = dataCount.m_lstAllData[0]["TYPE"].toString();
		infor.m_strName = dataCount.m_lstAllData[0]["GROUP_NAME"].toString();
		infor.pix = QPixmap::fromImage(im);

		CustomAddInformationWidget* pAddInfor = new CustomAddInformationWidget(infor, this);
		ui.tabFriendRecord->setCellWidget(i / COLUMN_COUNT, i % COLUMN_COUNT, pAddInfor);
	}
}


CustomAddInformationWidget::CustomAddInformationWidget(TargetInfor& infor, AbstractWidget* parent /*= 0*/) : 
	AbstractWidget(parent), m_Apply(NULL), m_Infor(infor)
{
	QString strExtra = m_Infor.m_IsFriend ? m_Infor.m_Gender + " " + QString::number(m_Infor.m_Age) + " " + m_Infor.m_strBirthPlace
		: QString::number(m_Infor.memberCount) + " " + m_Infor.m_strGroupType;
	m_BtnTgtInfo = new QToolButton(this);
	m_BtnTgtInfo->setObjectName("m_BtnTgtInfo");
	
	QFont f = this->font();
	f.setPointSize(12);
	m_BtnAddButton = new QPushButton("+", this);
	m_BtnAddButton->setFont(f);
	m_BtnTgtInfo->setIcon(QIcon(infor.pix.scaled(IMAGE_SIZE, IMAGE_SIZE)));
	m_BtnTgtInfo->setFont(f);
	m_BtnTgtInfo->setText(infor.m_strName + "(" + strExtra + ")");
	m_BtnTgtInfo->setToolTip(infor.m_strName + "(" + strExtra + ")");
	m_BtnTgtInfo->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	m_BtnTgtInfo->setAutoRaise(true);
	m_BtnTgtInfo->setIconSize(QSize(IMAGE_SIZE, IMAGE_SIZE));
	
	connect(m_BtnTgtInfo, SIGNAL(clicked()), this, SLOT(TgtInfor()));
	connect(m_BtnAddButton, SIGNAL(clicked()), this, SLOT(Add()));
	QGridLayout* lay = new QGridLayout;
	lay->addWidget(m_BtnTgtInfo, 0, 0, 1, 2);
	m_BtnTgtInfo->setSizePolicy(QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::MinimumExpanding);
	QSpacerItem* item = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
	lay->addItem(item, 1, 0);
	lay->addWidget(m_BtnAddButton, 1, 1);
	lay->setContentsMargins(6, 6, 6, 6);
	lay->setSpacing(0);
	setLayout(lay);
	if (m_Infor.m_strNumber == *FriendList::m_pUserNumber)
		m_BtnAddButton->hide();
}

CustomAddInformationWidget::CustomAddInformationWidget(AbstractWidget* parent /*= 0*/) : AbstractWidget(parent), m_Apply(NULL)
{
	
}

CustomAddInformationWidget::~CustomAddInformationWidget()
{

}

void CustomAddInformationWidget::paintEvent(QPaintEvent *event)
{
	QPainter pa(this);
	pa.begin(this);
	pa.setBrush(QBrush(QColor(30,100,100)));
	pa.drawRect(this->rect());
	AbstractWidget::paintEvent(event);
}

void CustomAddInformationWidget::TgtInfor()
{
	if (!m_UserWidget) {
		m_UserWidget = new AbstractWidget(this);
		QLabel* labImage = new QLabel(m_UserWidget);
		QLabel* labUserName = new QLabel(m_UserWidget);
		QLabel* labNumber = new QLabel(m_UserWidget);
		labImage->setPixmap(m_Infor.pix);
		labUserName->setText(m_Infor.m_strName);
		labNumber->setText(m_Infor.m_strNumber);
		QVBoxLayout* lay = new QVBoxLayout;
		lay->addWidget(labImage);
		lay->addWidget(labUserName);
		lay->addWidget(labNumber);

		if (m_Infor.m_IsFriend) {
			QLabel* labAge = new QLabel(m_UserWidget);
			QLabel* labSchool = new QLabel(m_UserWidget);
			QLabel* labBirthPlace = new QLabel(m_UserWidget);
			QLabel* labQccupational = new QLabel(m_UserWidget);
			QLabel* labGender = new QLabel(m_UserWidget);
			QLabel* labAddress = new QLabel(m_UserWidget);

			labAge->setText(QString::number(m_Infor.m_Age));
			labSchool->setText(m_Infor.m_strSchool);
			labBirthPlace->setText(m_Infor.m_strBirthPlace);
			labQccupational->setText(m_Infor.m_Occupational);
			labGender->setText(m_Infor.m_Gender);
			labAddress->setText(m_Infor.m_Address);

			lay->addWidget(labAge);
			lay->addWidget(labGender);
			lay->addWidget(labAddress);
			lay->addWidget(labBirthPlace);
			lay->addWidget(labSchool);
			lay->addWidget(labQccupational);
		}
		else {
			QLabel* labGroupType = new QLabel(m_UserWidget);
			QLabel* labMemberCount = new QLabel(m_UserWidget);
			labGroupType->setText(m_Infor.m_strGroupType);
			labMemberCount->setText(QString::number(m_Infor.memberCount));
			lay->addWidget(labMemberCount);
			lay->addWidget(labGroupType);
		}

		labImage->setMinimumSize(300, 300);
		m_UserWidget->setLayout(lay);
		SendSIG(Signal_::SHOW_ABSTRACTWIDGET, m_UserWidget);
	}
}

void CustomAddInformationWidget::Add()
{
	if (!m_Apply) {
		m_Apply = new ApplyJoinFriend(m_Infor, this);
		SendSIG(Signal_::SHOW_ABSTRACTWIDGET, m_Apply);
	}
}


