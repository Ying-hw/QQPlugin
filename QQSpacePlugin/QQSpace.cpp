#include "stdafx.h"
#include "QQSpace.h"

QQSpace::QQSpace(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	InitWidget();
}

QQSpace::~QQSpace()
{

}

void QQSpace::InitWidget()
{
	QWidget* spaceWidget = new QWidget(ui.listSpaceWidget);
}
