#ifndef   __SQLPLUGIN__
#define   __SQLPLUGIN__

#include "sqlplugin_global.h"
#include <QSqlDataBase>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QVariant>
#include <QThread>
#include <QHostInfo>

namespace sqlPlugin {

	struct SQLPLUGIN_EXPORT DataStructDefine {
		QList<QMap<QString, QVariant>> m_lstAllData;
		QString m_strError;
		bool m_Result;
		const DataStructDefine& operator=(const DataStructDefine& that);
		DataStructDefine(const DataStructDefine& target);
		DataStructDefine();
	};

	struct SQLPLUGIN_EXPORT DBSelect {
		 DataStructDefine& GetData(const QString &strSql);
		 bool ExecuteSql(const QString &strSql);
		 bool InsertImage(const QString &strSql, QVariant& ImageData);
		 QString m_strError;
	};

	class SQLPLUGIN_EXPORT DataLib : public QThread {
		Q_OBJECT
	public:

		~DataLib();

		void ConnectServer(QString strAddr, quint16 port);

		bool GetOpenResult();

		static DataLib* GetDataLibInstance();

		DBSelect* GetSelectInstance();

		void run();

		QString m_strError;

	public slots:
		void openDataLib(QHostInfo host);
	private:
		DataLib();
		DataLib(const DataLib&);
		const DataLib& operator=(const DataLib& target);
		bool m_IsOpen;
		bool hostIsError;
		quint32 m_Port;
		QString m_strDataBaseName;
		QString m_strAddress;
		QString m_strPassWD;
		QString m_strDataLibUserName;
		QSqlDatabase m_dataBase;
	};
}

#endif
