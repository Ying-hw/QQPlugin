#ifndef __CUSTOMTEXTEDIT__
#define __CUSTOMTEXTEDIT__

#include <QTextEdit>
#include <QDragEnterEvent>

class CustomTextEdit : public QTextEdit
{
	Q_OBJECT
public:
	CustomTextEdit(QWidget* parent = 0);
	~CustomTextEdit();

	void dragEnterEvent(QDragEnterEvent *event);

	void dropEvent(QDropEvent *event);

	const QStringList& GetFilePath();

	void clearCache();
private:
	QStringList m_strFilePath;
	QStringList m_strSuffixList;
};

#endif

