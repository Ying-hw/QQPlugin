#ifndef VIDEOCHAT_H
#define VIDEOCHAT_H

#include "AbstractWidget.h"
#include "ui_VideoChat.h"
#include "opencv.hpp"
#include "CustomVideoImage.h"

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

	/// \brief 查找用户的信息显示在界面中
	/// 来电提示
	/// \param[in] strNum 用户账号
	void FintTgtNum(const QString& strNum);

	/// \brief 绘制圆形图标头像
	/// \param[in] src 原图形
	/// \param[in] radius 圆角
	/// \retval 返回绘制好的
	QPixmap PixmapToRound(const QPixmap &src, int radius);

	/// \brief 关闭事件
	/// \param[in] event 系统参数
	void closeEvent(QCloseEvent *event);

	/// \brief 刷新对方图像
	/// \param[in] image 帧
	void RefreshImage(const QImage& image);

public slots:
	/// \brief 更新图像
	void SlotUpdateImage();

	void SlotConsent();

	void SlotRejective();

private:
	Ui::VideoChat ui;
	Mat m_cvSrvImage;             ///< 帧
	VideoCapture m_cvVideo;       ///< 视频采集
	ProcessNetWork* m_pNetWork;   ///< 接受网络数据
	QTimer* m_pTimer;             ///< 定时器刷新图像
	QString m_strSelf;            ///< 本人账号
	QString m_strTgtNum;		  ///< 对方账号
	CustomVideoImage* m_CustomImage; ///< 对方的视频窗口
};

#endif // VIDEOCHAT_H
