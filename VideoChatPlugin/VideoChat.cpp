#include "stdafx.h"
#include "VideoChat.h"
#include "HintFrameWidget.h"

VideoChat::VideoChat(QWidget *parent)
	: AbstractWidget(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_TranslucentBackground);
	if (m_cvVideo.open(0)) {
		m_cvSrvImage = Mat::zeros(m_cvVideo.get(CAP_PROP_FRAME_HEIGHT), m_cvVideo.get(CAP_PROP_FRAME_WIDTH), CV_8UC3);
	}
}

VideoChat::~VideoChat()
{

}

void VideoChat::initCamera()
{

}

void VideoChat::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	QImage im((uchar*)m_cvSrvImage.data, m_cvSrvImage.cols, m_cvSrvImage.rows, QImage::Format_RGB888);
	ui.LabVideoImage->setPixmap(QPixmap::fromImage(im));
	ui.LabVideoImage->resize(im.size());
}

void VideoChat::UpdateImage()
{
	m_cvVideo >> m_cvSrvImage;
	if (m_cvSrvImage.data) {
		cvtColor(m_cvSrvImage, m_cvSrvImage, COLOR_BGR2RGB);
		repaint();
	}
}

