#ifndef QQSPACE_H
#define QQSPACE_H

#include <QWidget>
#include "ui_QQSpace.h"

class QQSpace : public QWidget
{
	Q_OBJECT

public:
	QQSpace(QWidget *parent = 0);
	~QQSpace();
	void InitWidget();
private:
	Ui::QQSpace ui;
};

#endif // QQSPACE_H
