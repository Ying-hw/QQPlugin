#include "stdafx1.h"
#include "LoginSystem.h"
#include "HintFrameWidget.h"


#define    CONDIGIMAGEFILE   "../Data/Config/UserConfig.pr"
#define    CONDIGFILE   "../Data/Image/Avatar.jpg"

LoginSystem::LoginSystem(QWidget *parent):AbstractWidget(parent), m_BackPassWD(NULL), m_CurrentType(OperatorType::LOGIN){
	ui.setupUi(this);
	connect(ui.BtnLogin, SIGNAL(clicked()), this, SLOT(SlotLogin()));
	connect(ui.BtnRegister, SIGNAL(clicked()), this, SLOT(SlotRegister()));
	connect(ui.BtnBackPassWD, SIGNAL(clicked()), this, SLOT(SlotBackPassWD()));
	connect(GET_DATALIBTHREAD, SIGNAL(finished()), this, SLOT(OpenDataLibResult()));
	SetCurrenrtUser();
	RecoverImage(); 
	ui.BtnLogin->setShortcut(Qt::Key_Enter);
	ui.BtnLogin->setObjectName("BtnLogin");
	ui.BtnBackPassWD->setObjectName("BtnBackPassWD");
	ui.BtnRegister->setObjectName("BtnRegister");
}

void LoginSystem::processLoginResult(QHostInfo host)
{
	bool IsConnected = (host.error() == QHostInfo::NoError);
	if (IsConnected ) 
		OPEN_DATATBASE();
	else {
		HintFrameWidget* hint = new HintFrameWidget(QString::fromLocal8Bit("网络连接错误或者远端服务器未开启！！！"), this);
		hint->show();
	}
}

void LoginSystem::processRegisterResult(QHostInfo host)
{
	bool IsConnected = (host.error() == QHostInfo::NoError);
	if (!IsConnected) {
		HintFrameWidget* hint = new HintFrameWidget(QString::fromLocal8Bit("网络连接错误或者远端服务器未开启！！！"), this);
		hint->show();
		return;
	}
	OPEN_DATATBASE();
}

void LoginSystem::OpenDataLibResult()
{
	bool IsOpen = GET_OPENRESULT();
	if (IsOpen) {
		if (m_CurrentType == OperatorType::LOGIN) {
			sqlPlugin::DataStructDefine UserData = GET_DATA(QString(SELECT_USER).arg(ui.ComUserName->currentText()));
			if (UserData.m_lstAllData.isEmpty()) {
				HintFrameWidget* hint = new HintFrameWidget(QString::fromLocal8Bit("账号输入错误！！"), this);
				hint->show();
				return;
			}
			QCryptographicHash hash(QCryptographicHash::Md5);
			hash.addData(ui.EditPassWd->text().toLocal8Bit());
			QByteArray encryption = hash.result().toHex();

			if (UserData.m_lstAllData[0]["PASSWORD"].toString() != encryption) {
				HintFrameWidget* hint = new HintFrameWidget(QString::fromLocal8Bit("密码输入错误！！"), this);
				hint->show();
				return;
			}
			saveUserConfig();
			SEND_MESSAGE(new QString(UserData.m_lstAllData[0]["USER_NAME"].toString()), new QString(ui.ComUserName->currentText()));
			SendSIG(Signal_::SWITCHPLUGIN, "FriendListPlugin");
		}
		else if (m_CurrentType == OperatorType::REGISTER) {			//进入注册界面 
			SendSIG(Signal_::CLOSE, this);
			Register* reg = new Register(this);
			SendSIG(Signal_::SHOW_ABSTRACTWIDGET, reg);
		}
		else {}
	}
	else {
		HintFrameWidget* hint = new HintFrameWidget(QString::fromLocal8Bit("网络连接错误或者远端服务器未开启！！！"), this);
		hint->show();
	}
}

LoginSystem::~LoginSystem() {

}

void LoginSystem::SetCurrenrtUser()
{
	QFile file(QString(CONDIGIMAGEFILE));
	if (file.open(QIODevice::ReadOnly)) {
		QByteArray array = file.readAll();
		if (m_User.ParseFromString(array.data()) && !array.isEmpty())
			for (int i = 0; i < m_User.login_size();i++) {
				ui.ComUserName->addItem(QString::fromStdString(m_User.mutable_login(i)->usernumder()));
				if (m_User.mutable_login(i)->iscurrent())
					ui.ComUserName->setCurrentText(QString::fromStdString(m_User.mutable_login(i)->usernumder()));
			}
		file.close();
		}
}

void LoginSystem::RecoverImage()
{
	QFile file(CONDIGFILE);
	if (file.open(QIODevice::ReadOnly)) {
		ui.LabImage->setAlignment(Qt::AlignCenter);
		QPixmap pix(CONDIGFILE, file.readAll());
		ui.LabImage->setPixmap(PixmapToRound(pix, 70));
	}
}

QPixmap LoginSystem::PixmapToRound(const QPixmap &src, int radius)
{
	if (src.isNull())
 		return QPixmap();
	QSize size(2 * radius, 2 * radius);
	QBitmap mask(size);
	QPainter painter(&mask);
	painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing);
	painter.fillRect(0, 0, size.width(), size.height(), Qt::white);
	painter.setBrush(QColor(0, 0, 0));
	painter.drawRoundedRect(0, 0, size.width(), size.height(), 99, 99);
	QPixmap image = src.scaled(size, Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
	image.setMask(mask);
	return image;
}


void LoginSystem::saveUserConfig()
{
	bool isConfigedUser = false;
	for (int i = 0; i < m_User.login_size();i++) 
		if (m_User.mutable_login(i)->usernumder() == ui.ComUserName->currentText().toStdString()) {
			isConfigedUser = true;
			m_User.mutable_login(i)->set_iscurrent(true);
		}
		else 
			m_User.mutable_login(i)->set_iscurrent(false);
	if (!isConfigedUser) {
		User_loginUser* loginU = m_User.add_login();
		loginU->set_usernumder(ui.ComUserName->currentText().toStdString().c_str());
		loginU->set_iscurrent(true);
	}
	QFile file(QString(CONDIGIMAGEFILE));
	if (file.open(QIODevice::WriteOnly)) {
		file.write(m_User.SerializeAsString().c_str());
		file.close();
	}
}

void LoginSystem::SlotBackPassWD() {
	//hide();
	//进入找回密码界面
	
//	test();
	m_CurrentType = OperatorType::FINDPASSWORD;
}

void LoginSystem::SlotRegister() {
	m_CurrentType = OperatorType::REGISTER;
	QHostInfo::lookupHost("33a15e2655.qicp.vip", this, SLOT(processRegisterResult(QHostInfo)));
}

void LoginSystem::SlotLogin() {
	m_CurrentType = OperatorType::LOGIN;
	QHostInfo::lookupHost("33a15e2655.qicp.vip", this, SLOT(processLoginResult(QHostInfo)));
}
