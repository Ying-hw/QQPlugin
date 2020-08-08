#pragma once

#include "sqlplugin_global.h"
#include <QSqlDataBase>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QVariant>
#include <QThread>

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
	public:
		~DataLib();
		quint32 m_Port;
		QString m_strDataBaseName;
		QString m_strAddress;
		QString m_strPassWD;
		QString m_strDataLibUserName;
		QString m_strError;
		QSqlDatabase m_dataBase;
		void openDataLib();
		bool GetOpenResult();
		static DataLib* GetDataLibInstance();
		DBSelect* GetSelectInstance();
		void run();
	private:
		DataLib();
		DataLib(const DataLib&);
		const DataLib& operator=(const DataLib& target);
		bool m_IsOpen;
	};
}
