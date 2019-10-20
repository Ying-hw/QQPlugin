#include "stdafx1.h"
#include "LoginSystem.h"

LoginSystem::LoginSystem(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.BtnLogin, SIGNAL(clicked()), this, SLOT(SlotLogin()));
	connect(ui.BtnRegister, SIGNAL(clicked()), this, SLOT(SlotRegister()));
	connect(ui.BtnBackPassWD, SIGNAL(clicked()), this, SLOT(SlotBackPassWD()));
}

LoginSystem::~LoginSystem()
{

}

void LoginSystem::SlotBackPassWD()
{

}

void LoginSystem::SlotRegister()
{

}

void LoginSystem::SlotLogin()
{

}
