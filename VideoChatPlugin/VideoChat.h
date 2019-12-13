#ifndef VIDEOCHAT_H
#define VIDEOCHAT_H

#include <QWidget>
#include "ui_VideoChat.h"

class VideoChat : public QWidget
{
	Q_OBJECT

public:
	VideoChat(QWidget *parent = 0);
	~VideoChat();

private:
	Ui::VideoChat ui;
};

#endif // VIDEOCHAT_H
