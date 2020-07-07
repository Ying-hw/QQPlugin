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
};

#endif

