#ifndef FTPSHARES_H
#define FTPSHARES_H

#include "MacroDefine.h"
#include "ui_ftpShares.h"
#include "Animation.h"

class ftpShares : public Animation
{
	Q_OBJECT

public:
	ftpShares(QWidget *parent = 0);
	~ftpShares();

private:
	Ui::ftpShares ui;
};

#endif // FTPSHARES_H
