#include "stdafx.h"
#include "CustomVideoImage.h"

CustomVideoImage::CustomVideoImage(QWidget* parent /*= 0*/)
{

}

CustomVideoImage::~CustomVideoImage()
{

}

void CustomVideoImage::drawImage(const QImage& TgtImage)
{
	setPixmap(QPixmap::fromImage(TgtImage));
	resize(TgtImage.size());
}
