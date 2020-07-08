#ifndef __CUSTOMVIDEOIMAGE__
#define __CUSTOMVIDEOIMAGE__

/// \brief 自定义显示图像
class CustomVideoImage : public QLabel
{
public:
	/// \brief 构造函数
	/// \param[in] parent 父窗口
	CustomVideoImage(QWidget* parent = 0);

	/// \brief 析构函数
	~CustomVideoImage();

	/// \brief 画图像
	/// \param[in] TgtImage 图像
	void drawImage(const QImage& TgtImage);

	/// \brief 鼠标移动事件
	/// \param[in] event 系统参数
	void mouseMoveEvent(QMouseEvent *event);

	/// \brief 鼠标点击事件
	/// \param[in] event 系统参数
	void mousePressEvent(QMouseEvent *event);

private:
	QPoint m_point;  ///< 鼠标当前位置
	bool m_isPress;  ///< 点击标识
};
#endif

