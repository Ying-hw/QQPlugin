#include "stdafx.h"
#include "VideoChat.h"
#include "HintFrameWidget.h"
#include "ProcessNetWork.h"
#include "CustomVideoImage.h"
#include "MessageTemplate.h"
#include "NetProtocConfig.pb.h"

extern VideoChat* g_VideoChat = nullptr;
#define  SELECT_USER "SELECT * FROM user_account WHERE USER_NUMBER = '%1';"

VideoChat::VideoChat(QWidget *parent)
	: AbstractWidget(parent), m_pNetWork(new ProcessNetWork(AbstractNetWork::ProtoType::TCP, "33a15e2655.qicp.vip", 54813, this)),
	m_pTimer(NULL), m_CustomImage(NULL)
{
	ui.setupUi(this);
	m_CustomImage = new CustomVideoImage(this);
	m_CustomImage->show();
	g_VideoChat = this;
	setAttribute(Qt::WA_TranslucentBackground);
	m_pTimer = new QTimer(this);
	bool isInitiative = (bool)GET_MESSAGE(0);
	m_strSelf = *(QString*)GET_MESSAGE(1);
	m_strTgtNum = *(QString*)GET_MESSAGE(2);
	connect(m_pTimer, &QTimer::timeout, this, &VideoChat::SlotUpdateImage);
	connect(ui.BtnConsent, SIGNAL(clicked()), this, SLOT(SlotConsent()));
	connect(ui.BtnRejective, SIGNAL(clicked()), this, SLOT(SlotRejective()));
	if (isInitiative) {
		ui.BtnConsent->show();
		ui.BtnRejective->show();
		ui.LabState->hide();
		SlotConsent();
	}
	else {
		ui.BtnConsent->hide();
		ui.BtnRejective->hide();
		ui.LabState->show();
		FintTgtNum(m_strTgtNum);
	}
	ui.LabVideoImage->hide();
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
	protocol protype;
	ChatRecord* pRecord = protype.add_chatcontent();
	QByteArray array;
	QBuffer buffer(&array);
	buffer.open(QIODevice::WriteOnly);
	im.save(&buffer);
	pRecord->set_content(buffer.data().toStdString());
	pRecord->set_type(ChatRecord_contenttype::ChatRecord_contenttype_video);
	pRecord->set_selfnumber(m_strSelf.toStdString());
	pRecord->set_targetnumber(m_strTgtNum.toStdString());
	m_pNetWork->Send(pRecord->SerializeAsString());
}

void VideoChat::FintTgtNum(const QString& strNum)
{
	QString strSelectUser = QString(SELECT_USER).arg(strNum);
	sqlPlugin::DataStructDefine data;
	GET_DATA(data, strSelectUser);
	if (!data.m_lstAllData.isEmpty()) {
		QString strName = data.m_lstAllData[0]["USER_NAME"].toString();
		QByteArray array = data.m_lstAllData[0]["IMAGE"].toByteArray();
		QPixmap pix;
		if (pix.loadFromData(array)) {
			QPixmap newpix = pix.scaled(100, 100, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
			newpix = PixmapToRound(newpix, 50);
			ui.BtnFriendInfo->setIcon(newpix);
			ui.BtnFriendInfo->setText(strName);
			ui.BtnFriendInfo->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
			ui.BtnFriendInfo->setAutoRaise(true);
		}
	}
}

QPixmap VideoChat::PixmapToRound(const QPixmap &src, int radius)
{
	if (src.isNull())
		return QPixmap();
	QSize size(2 * radius, 2 * radius);
	QBitmap mask(size);
	QPainter painter(&mask);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::SmoothPixmapTransform);
	painter.fillRect(0, 0, size.width(), size.height(), Qt::white);
	painter.setBrush(QColor(0, 0, 0));
	painter.drawRoundedRect(0, 0, size.width(), size.height(), 99, 99);
	QPixmap image = src.scaled(size);
	image.setMask(mask);
	return image;
}

void VideoChat::closeEvent(QCloseEvent *event)
{
	if (m_cvVideo.isOpened())
		m_cvVideo.release();
}

void VideoChat::RefreshImage(const QImage& image)
{
	m_CustomImage->drawImage(image);
	ui.HintWidget->hide();
	ui.LabVideoImage->show();
}

void VideoChat::SlotUpdateImage()
{
	m_cvVideo >> m_cvSrvImage;
	if (m_cvSrvImage.data) {
		cvtColor(m_cvSrvImage, m_cvSrvImage, COLOR_BGR2RGB);
		repaint();
	}
}

void VideoChat::SlotConsent()
{
	if (!m_cvVideo.isOpened() && m_cvVideo.open(0)) {
		m_cvSrvImage = Mat::zeros(m_cvVideo.get(CAP_PROP_FRAME_HEIGHT), m_cvVideo.get(CAP_PROP_FRAME_WIDTH), CV_8UC3);
		ui.HintWidget->hide();
		ui.LabVideoImage->show();
		m_pTimer->start(33);
	}
}

void VideoChat::SlotRejective()
{
	SendSIG(Signal_::FREEPLUG, "VideoChatPlugin");
}

