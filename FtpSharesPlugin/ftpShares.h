#ifndef FTPSHARES_H
#define FTPSHARES_H

#include <QWidget>
#include "ui_ftpShares.h"

class ftpShares : public QWidget
{
	Q_OBJECT

public:
	ftpShares(QWidget *parent = 0);
	~ftpShares();

private:
	Ui::ftpShares ui;
};

#endif // FTPSHARES_H
