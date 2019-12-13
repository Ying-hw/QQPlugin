#include "stdafx1.h"
#include "SignalQueue.h"
#include "Register.h"

Register::Register(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	SENDMESSAGE(Signal_::RELOADUI, this);
}

Register::~Register()
{
	
}

