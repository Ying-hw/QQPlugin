#include "stdafx.h"
#include "CustomTextEdit.h"

CustomTextEdit::CustomTextEdit(QWidget* parent /*= 0*/) : QTextEdit(parent)
{

}

CustomTextEdit::~CustomTextEdit()
{

}

void CustomTextEdit::dragEnterEvent(QDragEnterEvent *event)
{
	if (!isReadOnly())
		clear();
	setReadOnly(false);
	event->acceptProposedAction();
	QTextEdit::dragEnterEvent(event);
}

void CustomTextEdit::dropEvent(QDropEvent *event)
{
	QList<QUrl> lsturl = event->mimeData()->urls();
	foreach(QUrl ur, lsturl) {
		QFile f(ur.toLocalFile());
		m_strFilePath << ur.toLocalFile();
		QFileInfo fInfo(f);
		QFileIconProvider ficon;
		QIcon ic = ficon.icon(fInfo);
		QImage imIcon = ic.pixmap(QSize(40, 40)).toImage();
		textCursor().insertImage(imIcon);
	}
	setReadOnly(true);
	QTextEdit::dropEvent(event);
}

const QStringList& CustomTextEdit::GetFilePath()
{
	return m_strFilePath;
}

void CustomTextEdit::clearCache()
{
	m_strFilePath.clear();
}
