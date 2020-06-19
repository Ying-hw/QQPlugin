#ifndef REGISTER_H
#define REGISTER_H


#include "ui_Register.h"
#include "MacroDefine.h"
#include "SqlStatementDefine.h"
#include "AbstractWidget.h"

enum GenDer { MAN, WOMAN };
enum StackedWidgetPage { USERINFO,IDENTIFY,ACCOUNT };
enum forwordNext { FORWORD,NEXT };

class Register : public AbstractWidget
{
	Q_OBJECT

public:
	Register(QWidget *parent = 0);
	~Register();

private slots:
	void NextPage();
	void LastPage();
	void RegisterUser();
	void FinishAnimationAfter();
	QByteArray LoadDefaultImage();
private:
	void AddUserToSqldatabase();
	bool CheckInputIsEmpty(int nIndex_Widget);
	void SetAnimation();
private:
	Ui::Register ui;
	QList<QPropertyAnimation*> m_pArrayAnimation;
	int m_widgetLocation;
	forwordNext m_direction;
};

#endif // REGISTER_H
