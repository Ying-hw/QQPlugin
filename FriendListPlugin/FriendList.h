#ifndef FRIENDLIST_H
#define FRIENDLIST_H

#ifndef CHATMESSAGE_H
#include "ui_FriendList.h"
#endif
#include "MacroDefine.h"
#include "friendlistplugin_global.h"
#include "AbstractNetWork.h"
#include "NetProtocConfig.pb.h"
#include "SqlStatementDefine.h"
#include "AbstractWidget.h"
#include "ProsessMessage.h"


/// \brief �Զ��尴ť����
/// ��Ҫ��δ����Ϣ�Ļ�����
class FRIENDLISTPLUGIN_EXPORT CustomToolButton : public QToolButton 
{
	Q_OBJECT
public:
	/// \brief ���캯��
	/// \param[in] parent ������
	CustomToolButton(QWidget* parent = 0);

	/// \brief ��������
	~CustomToolButton();

	CustomToolButton(const CustomToolButton* button);

	const CustomToolButton& operator=(const CustomToolButton* button);

	/// \brief ��ͼ�¼�
	/// \param[in] event ϵͳ����
	void paintEvent(QPaintEvent *event);

	/// \brief ���û�����������
	/// \param[in] strContent ����
	void SetPaintContent(QString strContent);

	/// \brief ��ȡ��ͼ������
	QString& GetPaintContent();
private:
	QString m_strPaintContent; ///< ��������
};

/// \brief ����������֤
class CustomAddFriendMessageHint : public AbstractWidget
{
	Q_OBJECT
public:
	/// \brief ���캯��         
	/// \param[in] parent ������
	CustomAddFriendMessageHint(AbstractWidget* parent = 0);

	/// \brief ��������
	~CustomAddFriendMessageHint();

	/// \brief ���ò���
	/// \param[in] infor ������Ϣ
	void SetLayout(AddInformation* infor);

private slots:
	/// \brief ͬ�����
	void ConsentApply();

	/// \brief ��ʾ������Ϣ
	void UserInfor();
private:
	AbstractWidget* m_UserWidget;  ///< ������Ϣ
	bool m_IsAddFriend; ///< ���Ⱥ���Ǻ���
	bool m_Gender; ///< �Ա� 
	AddInformation m_infor;   ///< �����Ϣ 
};

class FRIENDLISTPLUGIN_EXPORT FriendList : public AbstractWidget
{
	Q_OBJECT

public:
	FriendList(QWidget *parent = 0);
	~FriendList();
	/// \brief ����¼��ʱ��ָ���������
	void RecoveryChatRecord();

	/// \brief ���ú����б����
	/// \param[in] strNum �����˺�
	/// \param[in] strName ���ѵ��û�����
	/// \param[in] byteImage ���ѵ�ͷ��
	/// \param[in] isFriend ����
	void SetMessage_ListUi(const QString& strNum, const QString& strName, const QByteArray& byteImage, bool isFriend);

	/// \brief ��ʼ�������б�
	void InitFriendList();

	/// \brief ��ʼ��Ⱥ��
	void InitGroupList();

	/// \brief ��ʼ��QQ�ռ�
	void InitQQSpaceList();

	/// \brief ����Բ��ͼ��ͷ��
	/// \param[in] src ԭͼ��
	/// \param[in] radius Բ��
	/// \retval ���ػ��ƺõ�
	static QPixmap PixmapToRound(const QPixmap &src, int radius);

	/// \brief ��ʾδ֪��Ϣ������
	/// \param[in] strTgtNum Ŀ���˺�
	/// \param[in] isFriend �Ƿ��Ǻ���
	void ShowUnknownMsgCount(const QString& strTgtNum, bool isFriend);

	/// \brief ������Ƶ����
	/// \param[in] strNum �Է��˺�
	void StartVideoChat(const QString& strNum);

	/// \brief ������Ӻ��ѵ�����
	/// \param[in] infor �Է���Ϣ
	void RecvFriendApply(AddInformation* infor);

	/// \brief �����˵�λ��
	/// \param[in] event ϵͳ����
	void paintEvent(QPaintEvent *event);

	/// \brief ���º��ѵ�״̬
	/// \param[in] strNum �����˺�
	/// \param[in] state ״̬��Ϣ
	void UpdateFriendState(const QString strNum, StateInformation state);

	/// \brief ��ɫͷ��ת���Ҷ�ͼ��
	/// \param[in] icon ͼ��Դ
	/// \param[in] size ��С
	/// \retval ����ת����ĻҶ�ͷ��
	static QImage convertImage(QIcon iconSource, QSize size);

	/// \brief ���������¼
	/// \param[in] proto Э��
	void SaveChatRecord(protocol& proto);

	/// \brief ��ȡ��ǰ״̬
	/// \param[in] proto ԭЭ��
	/// \retval ���ص�ǰ״̬
	static void setCurrentState(protocol& proto);

	static QString* m_pUserNumber;    ///< �����˺�
	static ProsessMessage* m_NetWorkProsess;  ///< ����������Ϣ

signals:
	/// \brief ʹ���źų�ʼ�����԰�������������ʾ�ٶȣ�������
	void InitAllMember();

private slots:

	/// \brief ��ʼ����Ա
	void Initialization();

	/// \brief ��ʼ����
	void SlotStartChat();

	/// \brief Ⱥ������
	void SlotStartGroupChat();

	/// \brief �����Ϣ�Ĳۺ���
	/// �����Ϣ��ʼ����
	void SlotStartChatFromMessage();

	/// \brief ����Ϣ���ȡ��Ϣ���ҿ�ʼȺ������
	void SlotStartGroupChatFromMessage();

	/// \brief ������ѻ���Ⱥ��
	/// \param[in] isClicked �����ʶ
	void SlotAdd(bool isClicked = false);

	/// \brief �л������б���Ϣ�б�ռ�
	void SwitchFriMsgSpace();

	/// \brief ״̬�ı䣨���ߣ����ߣ��������ţ�
	/// \param[in] strCurrentText ��ǰ״̬
	void SlotChangedState(const QString &strCurrentText);

private:
#ifndef CHATMESSAGE_H
	Ui::FriendList ui;
#endif
	QMap<CustomToolButton *, QString> m_mapGroup;    ///< Ⱥ��ӳ��
	QMap<CustomToolButton *, QString> m_mapFriend;   ///< ����ӳ��
	QMap<CustomToolButton *, QString> m_mapMesssage;  ///< ��Ϣӳ��
	QMenu* m_pSystemMenu;  ///< ϵͳ�˵�
	QTreeWidgetItem* m_pFriendTree; ///< �����б�
	QTreeWidgetItem* m_pGroupTree;  ///< Ⱥ���б�
};


#endif // FRIENDLIST_H
