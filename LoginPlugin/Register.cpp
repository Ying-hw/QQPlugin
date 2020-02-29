#include "stdafx1.h"
#include "SignalQueue.h"
#include "Register.h"
#include <typeinfo>

Register::Register(QWidget *parent)
	: Animation(parent)
{
	ui.setupUi(this);
	connect(ui.BtnReturn, &QPushButton::clicked, [this](){
		SEND_SIGNAL(Signal_::RELOADUI, this->parentWidget());
	});
}

Register::~Register()
{
	
}

