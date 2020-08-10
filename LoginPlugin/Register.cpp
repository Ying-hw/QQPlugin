#include "stdafx1.h"
#include "Register.h"
#include "Message.h"


#define ANIMATION_TIME  300
#define PASSWORDLENGTH  10
#define    CONDIG_IMAGE_FILE   "../Data/Image/Avatar.jpg"

Register::Register(AbstractWidget *parent) : AbstractWidget(parent), m_pArrayAnimation{}, m_widgetLocation(-1)
{
	ui.setupUi(this);
	connect(ui.BtnReturn, &QPushButton::clicked, [parent, this](){
		SendSIG(Signal_::CLOSE, this);
		SendSIG(Signal_::SHOW_ABSTRACTWIDGET, parent);
	});
	connect(ui.BtnReturn_2, &QPushButton::clicked, [parent, this]() {
		SendSIG(Signal_::CLOSE, this);
		SendSIG(Signal_::SHOW_ABSTRACTWIDGET, parent);
	});
	connect(ui.BtnReturn_3, &QPushButton::clicked, [parent, this]() {
		SendSIG(Signal_::CLOSE, this);
		SendSIG(Signal_::SHOW_ABSTRACTWIDGET, parent);
	});
	connect(ui.BtnPerious, SIGNAL(clicked()), this, SLOT(LastPage()));
	connect(ui.BtnPerious_2, SIGNAL(clicked()), this, SLOT(LastPage()));
	connect(ui.BtnPerious_3, SIGNAL(clicked()), this, SLOT(LastPage()));
	connect(ui.BtnNext, SIGNAL(clicked()), this, SLOT(NextPage()));
	connect(ui.BtnNext_2, SIGNAL(clicked()), this, SLOT(NextPage()));
	connect(ui.BtnAffrime, SIGNAL(clicked()), this, SLOT(RegisterUser()));
	QRegExp reg("(\\d|[a-z]|[A-Z]){1,}");
	ui.EditUserNumber->setValidator(new QRegExpValidator(reg, this));
	ui.EditIdentify->setValidator(new QRegExpValidator(reg, this));
	ui.BtnPerious->setEnabled(false);
	SetAnimation();
}

Register::~Register()
{
	for (QPropertyAnimation* ani: m_pArrayAnimation) {
		delete ani;
		ani = NULL;
	}
}

void Register::NextPage()
{
	ui.BtnPerious->setEnabled(true);
	int nextIndex = ui.stackedWidget->currentIndex() + 1;
	if (!CheckInputIsEmpty(nextIndex - 1))
		return;
	QRect currentRect = ui.stackedWidget->currentWidget()->geometry();
	QRect newRectLeft = currentRect;
	newRectLeft.moveLeft(currentRect.x() - currentRect.width());
	m_pArrayAnimation[nextIndex - 1]->setStartValue(currentRect);
	m_pArrayAnimation[nextIndex - 1]->setEndValue(newRectLeft);
	m_pArrayAnimation[nextIndex - 1]->setDuration(ANIMATION_TIME);
	m_direction = NEXT;
	connect(m_pArrayAnimation[nextIndex - 1], SIGNAL(finished()), this, SLOT(FinishAnimationAfter()));
	m_widgetLocation = nextIndex == ui.stackedWidget->count() ? 0 : nextIndex;
	m_pArrayAnimation[nextIndex - 1]->start();
}

void Register::LastPage()
{
	int lastIndex = ui.stackedWidget->currentIndex() - 1;
	if (lastIndex == USERINFO) 
		ui.BtnPerious->setEnabled(false);
	QRect currentRect = ui.stackedWidget->currentWidget()->geometry();
	QRect newRectRight = currentRect;
	newRectRight.moveLeft(currentRect.x() + currentRect.width());
	m_pArrayAnimation[lastIndex + 1]->setStartValue(currentRect);
	m_pArrayAnimation[lastIndex + 1]->setEndValue(newRectRight);
	m_pArrayAnimation[lastIndex + 1]->setDuration(ANIMATION_TIME);
	connect(m_pArrayAnimation[lastIndex + 1], SIGNAL(finished()), this, SLOT(FinishAnimationAfter()));
	m_direction = FORWORD;
	m_widgetLocation = lastIndex == -1 ? ui.stackedWidget->count() - 1 : lastIndex;
	m_pArrayAnimation[lastIndex + 1]->start();
}

void Register::RegisterUser()
{
	QString strPassWord = ui.EditPassword->text();
	QString strAgainPassWord = ui.EditAgainInput->text();
	if (ui.EditUserNumber->text().isEmpty()) {
		QMessageBox::warning(this, QString::fromLocal8Bit("错误"),
			QString::fromLocal8Bit("请输入账号"));
		return;
	}
	if (strPassWord.isEmpty()) {
		QMessageBox::warning(this, QString::fromLocal8Bit("警告"),
			QString::fromLocal8Bit("请输入密码！"));
		return;
	}
	if (strPassWord.length() < PASSWORDLENGTH || strAgainPassWord < PASSWORDLENGTH) {
		QMessageBox::warning(this, QString::fromLocal8Bit("警告"),
			QString::fromLocal8Bit("密码长度不够，请重新输入！"));
		return;
	}
	if (strPassWord != strAgainPassWord) {
		QMessageBox::warning(this, QString::fromLocal8Bit("警告"),
			QString::fromLocal8Bit("密码输入错误，请重新输入！"));
		return;
	}
	AddUserToSqldatabase();
}

void Register::FinishAnimationAfter()
{
	disconnect(sender(), SIGNAL(finished()), this, SLOT(FinishAnimationAfter()));
	ui.stackedWidget->setCurrentIndex(m_widgetLocation);
	QRect currentRect = ui.stackedWidget->currentWidget()->geometry();
	QRect newRect = currentRect;
	if (m_direction == NEXT) 
		newRect.moveLeft(currentRect.x() + currentRect.width());
	else 
		newRect.moveLeft(currentRect.x() - currentRect.width());
	m_pArrayAnimation[m_widgetLocation]->setStartValue(newRect);
	m_pArrayAnimation[m_widgetLocation]->setEndValue(currentRect);
	m_pArrayAnimation[m_widgetLocation]->setDuration(ANIMATION_TIME);
	m_pArrayAnimation[m_widgetLocation]->start();
	update();
	repaint();
	connect(sender(), SIGNAL(finished()), this, SLOT(FinishAnimationAfter()));
}

QByteArray Register::LoadDefaultImage()
{ 
	QFile file(QString(CONDIG_IMAGE_FILE));
	if (file.open(QIODevice::ReadOnly)) {
		QByteArray arrayImage = file.readAll();
		file.close();
		return arrayImage;
	}
	return QByteArray();
}

void Register::AddUserToSqldatabase()
{
	QString strUserName = ui.EditName->text();
	GenDer gender = (GenDer)ui.ComGender->currentIndex();
	int age = ui.SpinAge->value();
	QString strAddress = ui.ComProvinces->currentText();
	QString strOccupational = ui.ComOccupational->currentText();

	QString strBirthplace = ui.EditBirthplace->text();
	QString strSchool = ui.EditSchool->text();
	QString strIdentify = ui.EditIdentify->text();

	QString strAccount = ui.EditUserNumber->text();
	QString strPassWord = ui.EditPassword->text();
	QCryptographicHash hash(QCryptographicHash::Md5);
	hash.addData(strPassWord.toLocal8Bit());
	QByteArray encryption = hash.result().toHex();
	QString strRegisterUserSql = QString(INSERT_USER).arg(encryption.data()).arg(strAccount).arg(strUserName)
		.arg(gender).arg(age).arg(strAddress)
		.arg(strOccupational).arg(strBirthplace)
		.arg(strSchool).arg(strIdentify);
	QString strInsertState = QString(INSERT_FRIENDSTATE).arg(ui.EditUserNumber->text()).arg(QString::fromLocal8Bit("在线"));

	sqlPlugin::DataStructDefine& dataKey = GET_DATA(QString(SELECT_USER).arg(strAccount));
	if (!dataKey.m_lstAllData.isEmpty())
	{
		HintFrameWidget* hint = new HintFrameWidget(QString::fromLocal8Bit("账号已经注册，请更换！"), this);
		hint->show();
		return;
	}


	if (EXECUTE(strRegisterUserSql) && UPDATE_IMAGE(QString(UPDATEIMAGE).arg("user_account").arg(strAccount), QVariant(LoadDefaultImage())) && EXECUTE(strInsertState)) {
		QMessageBox::information(this, QString::fromLocal8Bit("成功"), QString::fromLocal8Bit("注册成功，正在回到登录界面，请重新登录"));
		QThread::sleep(1);
		ui.BtnReturn->click();
	}
	else 
		QMessageBox::warning(this, QString::fromLocal8Bit("错误"),
			QString::fromLocal8Bit("网络连接失败！"));
}

bool Register::CheckInputIsEmpty(int nIndex_Widget)
{
	switch (nIndex_Widget)
	{
	case USERINFO:
		if (ui.EditName->text().isEmpty()) {
			QMessageBox::warning(this, QString::fromLocal8Bit("错误"),
				QString::fromLocal8Bit("请输入用户名"));
			return false;
		}
		break;
	case IDENTIFY:
		if (ui.EditBirthplace->text().isEmpty()) {
			QMessageBox::warning(this, QString::fromLocal8Bit("错误"),
				QString::fromLocal8Bit("请输入出生地"));
			return false;
		}
		if (ui.EditSchool->text().isEmpty()) {
			QMessageBox::warning(this, QString::fromLocal8Bit("错误"),
				QString::fromLocal8Bit("请输入毕业院校"));
			return false;
		}
		if (ui.EditIdentify->text().isEmpty()) {
			QMessageBox::warning(this, QString::fromLocal8Bit("错误"),
				QString::fromLocal8Bit("请输入身份证号"));
			return false;
		}
		break;
	case ACCOUNT:
		break;
	default:
		break;
	}
	return true;
}

void Register::SetAnimation()
{
	for (int i = 0; i <= ACCOUNT;i++) {
		QPropertyAnimation* pOpacity = new QPropertyAnimation(ui.stackedWidget, "geometry", this);
		m_pArrayAnimation.append(pOpacity);
	}
}

