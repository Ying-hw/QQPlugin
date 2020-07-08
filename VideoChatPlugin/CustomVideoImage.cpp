#include "stdafx.h"
#include "CustomVideoImage.h"

CustomVideoImage::CustomVideoImage(QWidget* parent /*= 0*/)
{
	setWindowFlags(Qt::FramelessWindowHint);
}

CustomVideoImage::~CustomVideoImage()
{

}

void CustomVideoImage::drawImage(const QImage& TgtImage)
{
	setPixmap(QPixmap::fromImage(TgtImage));
	resize(TgtImage.size());
}

void CustomVideoImage::mouseMoveEvent(QMouseEvent *event)
{
	if (m_isPress)
		move(mapToGlobal(event->pos() - m_point));
}

void CustomVideoImage::mousePressEvent(QMouseEvent *event)
{
	QPoint point = event->pos();
	if (this->contentsRect().contains(point)) {
		m_point = point;
		m_isPress = true;
	}
	else
		m_isPress = false;
}
