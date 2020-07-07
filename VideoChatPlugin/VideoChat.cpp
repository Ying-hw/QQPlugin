#include "stdafx.h"
#include "VideoChat.h"
#include "HintFrameWidget.h"
#include "ProcessNetWork.h"
#include "CustomVideoImage.h"

extern VideoChat* g_VideoChat = nullptr;

VideoChat::VideoChat(QWidget *parent)
	: AbstractWidget(parent), m_pNetWork(new ProcessNetWork(AbstractNetWork::ProtoType::TCP, QHostAddress("192.168.1.17"), 7007, this))
{
	ui.setupUi(this);
	g_VideoChat = this;
	setAttribute(Qt::WA_TranslucentBackground);
	QTimer* pTimer = new QTimer(this);
	connect(pTimer, &QTimer::timeout, this, &VideoChat::UpdateImage);
	if (m_cvVideo.open(0)) {
		m_cvSrvImage = Mat::zeros(m_cvVideo.get(CAP_PROP_FRAME_HEIGHT), m_cvVideo.get(CAP_PROP_FRAME_WIDTH), CV_8UC3);
		pTimer->start(33);
	}
}

VideoChat::~VideoChat()
{

}

void VideoChat::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	QImage im((uchar*)m_cvSrvImage.data, m_cvSrvImage.cols, m_cvSrvImage.rows, QImage::Format_RGB888);
	im = im.mirrored(true, false);
	ui.LabVideoImage->setPixmap(QPixmap::fromImage(im));
	ui.LabVideoImage->resize(im.size());
	m_pNetWork->SendMsg(im);
}

void VideoChat::ShowTgtImage(const QImage& TgtImage)
{
	if (!TgtImage.isNull()) {
		static CustomVideoImage* pCusVideo = new CustomVideoImage(this);
		pCusVideo->drawImage(TgtImage);
	}
}

void VideoChat::UpdateImage()
{
	m_cvVideo >> m_cvSrvImage;
	if (m_cvSrvImage.data) {
		cvtColor(m_cvSrvImage, m_cvSrvImage, COLOR_BGR2RGB);
		repaint();
	}
}

