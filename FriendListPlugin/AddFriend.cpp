#include "stdafx.h"
#include "AddFriend.h"
#include "MacroDefine.h"
#include "SqlStatementDefine.h"
#include "ApplyJoinFriend.h"

#define IMAGE_SIZE  80
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
	sqlPlugin::DataStructDefine& data = GET_DATA(strSql);
	if (!data.m_lstAllData.isEmpty()) {
		int count = data.m_lstAllData.size();
		ui.tabFriendRecord->setRowCount(!(count % COLUMN_COUNT) ? count / COLUMN_COUNT : count / COLUMN_COUNT + 1);
	}
	for (int i = 0; i < data.m_lstAllData.size(); i++) {
		QString strGroupName = data.m_lstAllData[i][m_isAddFriend ? "USER_NAME" : "GROUP_NAME"].toString(), strExtraParam;
		QByteArray array = data.m_lstAllData[i]["IMAGE"].toByteArray();
		if (!m_isAddFriend) {
			QString strCount = QString(SELECT_GROUP_PEOPLECOUNT).arg(data.m_lstAllData[i]["GROUP_ACCOUNT"].toString());
			sqlPlugin::DataStructDefine& dataCount = GET_DATA(strCount);
			strExtraParam = data.m_lstAllData[i]["GROUP_ACCOUNT"].toString() + " " + QString::number(dataCount.m_lstAllData.size()) + QString::fromLocal8Bit("人");
		}
		else {
			QString strUser = QString(SELECT_USER).arg(data.m_lstAllData[i]["USER_NUMBER"].toString());
			sqlPlugin::DataStructDefine& UserData = GET_DATA(strUser);
			strExtraParam = UserData.m_lstAllData[0]["USER_NUMBER"].toString();
		}
		CustomAddInformationWidget* pAddInfor = new CustomAddInformationWidget(strGroupName, strExtraParam, array, m_isAddFriend, this);
		ui.tabFriendRecord->setCellWidget(i / COLUMN_COUNT, i % COLUMN_COUNT, pAddInfor);
	}
}

void AddFriend::SlotClickFriend(int row, int col)
{
	
	
}

void AddFriend::SlotGroupType()
{
	QPushButton *button = qobject_cast<QPushButton*>(sender());
	QString strSelect = QString(SELECT_GROUP_TYPE).arg(button->text());
	sqlPlugin::DataStructDefine& data = GET_DATA(strSelect);
	if (!data.m_lstAllData.isEmpty()) {
		int count = data.m_lstAllData.size();
		ui.tabFriendRecord->setRowCount(!(count % COLUMN_COUNT) ? count / COLUMN_COUNT : count / COLUMN_COUNT + 1);
	}
	for (int i = 0;i < data.m_lstAllData.size();i++) {
		QString strGroupName = data.m_lstAllData[i]["GROUP_NAME"].toString();
		QByteArray array = QString::fromUtf8(data.m_lstAllData[i]["IMAGE"].toByteArray()).toLocal8Bit();
		QString strCount = QString(SELECT_GROUP_PEOPLECOUNT).arg(data.m_lstAllData[i]["GROUP_ACCOUNT"].toString());
		sqlPlugin::DataStructDefine& dataCount = GET_DATA(strCount);
		QString strExtraParam  = data.m_lstAllData[i]["GROUP_ACCOUNT"].toString() + " " + QString::number(dataCount.m_lstAllData.size()) + QString::fromLocal8Bit("人");
		CustomAddInformationWidget* pAddInfor = new CustomAddInformationWidget(strGroupName, strExtraParam, array, false, this);
		ui.tabFriendRecord->setCellWidget(i / COLUMN_COUNT, i % COLUMN_COUNT, pAddInfor);
	}
}


CustomAddInformationWidget::CustomAddInformationWidget(QString strName, QString strAttr, QByteArray& arrayImage, bool isFriend, AbstractWidget* parent /*= 0*/) :
	m_strUserName(strName), m_ExtraParam(strAttr), AbstractWidget(parent), m_Apply(NULL), m_IsFriend(isFriend)
{
	m_BtnTgtInfo = new QToolButton(this);
	QFont f = this->font();
	f.setPointSize(14);
	m_BtnAddButton = new QPushButton("+", this);
	m_BtnAddButton->setFont(f);
	QImage image;
	if (image.loadFromData(arrayImage))
		m_Pixmap = QPixmap::fromImage(image);
	m_BtnTgtInfo->setIcon(QIcon(m_Pixmap.scaled(IMAGE_SIZE, IMAGE_SIZE)));
	m_BtnTgtInfo->setFont(f);
	m_BtnTgtInfo->setText(m_strUserName);
	m_BtnTgtInfo->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	m_BtnTgtInfo->setAutoRaise(true);
	m_BtnTgtInfo->setIconSize(QSize(IMAGE_SIZE, IMAGE_SIZE));
	connect(m_BtnTgtInfo, SIGNAL(clicked()), this, SLOT(TgtInfor()));
	connect(m_BtnAddButton, SIGNAL(clicked()), this, SLOT(Add()));
	QGridLayout* lay = new QGridLayout;
	lay->addWidget(m_BtnTgtInfo, 0, 0, 1, 2);
	m_BtnTgtInfo->setSizePolicy(QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::MinimumExpanding);
	QSpacerItem* item = new QSpacerItem(40, 20, QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::Fixed);
	lay->addItem(item, 1, 0);
	lay->addWidget(m_BtnAddButton, 1, 1);
	lay->setContentsMargins(0, 0, 0, 0);
	lay->setVerticalSpacing(0);
	lay->setHorizontalSpacing(0);
	setLayout(lay);
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

	QPen pen(QColor(100, 200, 40));
	pa.setPen(pen);
	

	QFont fName = this->font();
	QFont fAttr = this->font();
	fName.setPointSize(16);
	fAttr.setPointSize(10);
	pa.setFont(fName);

	pa.setBrush(QBrush(Qt::white));
	pa.drawRect(this->rect());

	QRect re = this->geometry();
	pa.drawText(IMAGE_SIZE + 20, re.center().y() - IMAGE_SIZE / 4, m_strUserName);
	pa.setFont(fAttr);
	pa.drawText(IMAGE_SIZE + 20, re.center().y() + IMAGE_SIZE / 4, m_ExtraParam);
	pa.end();
	QWidget::paintEvent(event);
}

void CustomAddInformationWidget::SetAttribute(QString strName, QString strAttr, QByteArray& arrayImage)
{
	m_strUserName = strName;
	m_ExtraParam = strAttr;
	QImage image;
	if (image.loadFromData(arrayImage))
		m_Pixmap = QPixmap::fromImage(image); 
}

void CustomAddInformationWidget::TgtInfor()
{
	if (!m_UserWidget) {
		m_UserWidget = new AbstractWidget(this);
		QLabel* labImage = new QLabel(m_UserWidget);
		QLabel* labUserName = new QLabel(m_UserWidget);
		QLabel* labNumber = new QLabel(m_UserWidget);
		labImage->setPixmap(m_Pixmap);
		labUserName->setText(m_strUserName);
		labNumber->setText(m_ExtraParam.section(" ", 0, 0));
		QVBoxLayout* lay = new QVBoxLayout;
		lay->addWidget(labImage);
		lay->addWidget(labUserName);
		lay->addWidget(labNumber);
		labImage->setMinimumSize(300, 300);
		m_UserWidget->setLayout(lay);
		SendSIG(Signal_::SHOW_ABSTRACTWIDGET, m_UserWidget);
	}
}

void CustomAddInformationWidget::Add()
{
	if (!m_Apply) {
		m_Apply = new ApplyJoinFriend(m_strUserName, m_ExtraParam, m_Pixmap, m_IsFriend, this);
		SendSIG(Signal_::SHOW_ABSTRACTWIDGET, m_Apply);
	}
}
