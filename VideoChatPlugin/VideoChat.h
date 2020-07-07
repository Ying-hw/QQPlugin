#ifndef VIDEOCHAT_H
#define VIDEOCHAT_H

#include "AbstractWidget.h"
#include "ui_VideoChat.h"
#include "opencv.hpp"

using namespace cv;
class ProcessNetWork;

/// \brief 视频通话类，继承框架类
class VideoChat : public AbstractWidget
{
	Q_OBJECT

public:
	/// \brief 构造函数
	/// \param[in] parent 构造函数
	VideoChat(QWidget *parent = 0);

	/// \brief 析构函数
	~VideoChat();

	/// \brief 显示相机的每一帧
	/// \param[in] event 系统参数
	void paintEvent(QPaintEvent *event);

	/// \brief 接受视频流
	/// \param[in] TgtImage 帧
	void ShowTgtImage(const QImage& TgtImage);

public slots:
	/// \brief 更新图像
	void UpdateImage();

private:
	Ui::VideoChat ui;
	Mat m_cvSrvImage;             ///< 帧
	VideoCapture m_cvVideo;       ///< 视频采集
	ProcessNetWork* m_pNetWork;
};

#endif // VIDEOCHAT_H
