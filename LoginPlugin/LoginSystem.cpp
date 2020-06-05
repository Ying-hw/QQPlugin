#include "stdafx1.h"
#include "LoginSystem.h"
#define    CONDIGIMAGEFILE   "../Data/Config/UserConfig.pr"
#define    CONDIGFILE   "../Data/Config/Avatar.jpg"

LoginSystem::LoginSystem(QWidget *parent):QWidget(parent) {
	ui.setupUi(this);
	connect(ui.BtnLogin, SIGNAL(clicked()), this, SLOT(SlotLogin()));
	connect(ui.BtnRegister, SIGNAL(clicked()), this, SLOT(SlotRegister()));
	connect(ui.BtnBackPassWD, SIGNAL(clicked()), this, SLOT(SlotBackPassWD()));
	SetCurrenrtUser();
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
	RecoverImage();
}

void LoginSystem::RecoverImage()
{
	QString strGetImage = QString(SELECT_USER).arg(ui.ComUserName->currentText());
	sqlPlugin::DataStructDefine& data = GET_DATA(strGetImage);
	QByteArray arrayImage = data.m_lstAllData[0]["IMAGE"].toByteArray();
	if (!arrayImage.isEmpty()) {
		QFile file(QString(CONDIGFILE));
		if (file.open(QIODevice::WriteOnly)) {
			file.write(arrayImage.data());
			file.close(); 
			update();
			repaint();
		}
	}
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
	//Ω¯»Î’“ªÿ√‹¬ÎΩÁ√Ê
	
//	test();

}

void LoginSystem::SlotRegister() {
	//hide();
	//Ω¯»Î◊¢≤·ΩÁ√Ê 
	SEND_SIGNAL(Signal_::RELOADUI, new Register(this));
}

void LoginSystem::SlotLogin() {
	if (OPEN_DATATBASE()) {
		sqlPlugin::DataStructDefine UserData = GET_DATA(QString(SELECT_USER).arg(ui.ComUserName->currentText()));
		if (UserData.m_lstAllData.isEmpty()) {
			QMessageBox::warning(this, QString::fromLocal8Bit("¥ÌŒÛ"),
				QString::fromLocal8Bit("’À∫≈ ‰»Î¥ÌŒÛ£°"));
			return;
		} 
		QCryptographicHash hash(QCryptographicHash::Md5);
		hash.addData(ui.EditPassWd->text().toLocal8Bit());
		QByteArray encryption = hash.result().toHex();

		if (UserData.m_lstAllData[0]["PASSWORD"].toString() != encryption) {
			QMessageBox::warning(this, QString::fromLocal8Bit("¥ÌŒÛ"),
				QString::fromLocal8Bit("√‹¬Î ‰»Î¥ÌŒÛ£°"));
			return;
		}
		saveUserConfig();
		SEND_MESSAGE(new QString(UserData.m_lstAllData[0]["USER_NAME"].toString()),
						new QString(ui.ComUserName->currentText()));
		SEND_SIGNAL(Signal_::SWITCHPLUGIN, "FriendListPlugin");
	} 
	else
		QMessageBox::warning(this, QString::fromLocal8Bit("¥ÌŒÛ"),
			QString::fromLocal8Bit("Õ¯¬Á¡¨Ω” ß∞‹£°"));
}

