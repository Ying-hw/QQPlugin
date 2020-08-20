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
#include <QMutex>

namespace sqlPlugin {
	template<typename T> struct MyList : QList<T> {
		const T& operator[](int index) const;
		int size() const;
		bool isEmpty() const;
	};

	template<typename T>
	bool sqlPlugin::MyList<T>::isEmpty() const
	{
		DataLib::GetDataLibInstance()->GetSelectInstance()->wait();
		return QList<T>::isEmpty();
	}

	template<typename T>
	int sqlPlugin::MyList<T>::size() const
	{
		DataLib::GetDataLibInstance()->GetSelectInstance()->wait();
		return QList<T>::size();
	}

	template<typename T>
	const T& sqlPlugin::MyList<T>::operator[](int index) const
	{
			DataLib::GetDataLibInstance()->GetSelectInstance()->wait();
			if (index < this->size() && index >= 0) {
				return this->at(index);
			}
			return T();
	}

	struct SQLPLUGIN_EXPORT DataStructDefine {
		MyList<QMap<QString, QVariant>> m_lstAllData;
		QString m_strError;
		bool m_Result;
		const DataStructDefine& operator=(const DataStructDefine& that);
		DataStructDefine(const DataStructDefine& target);
		DataStructDefine();
	};

	struct SQLPLUGIN_EXPORT DBSelect : QThread {
	private:
		Q_OBJECT
	public:
		DBSelect(QObject* parent = 0);
		~DBSelect();
		 void GetData(DataStructDefine& sourceData, const QString &strSql);
		 bool ExecuteSql(const QString &strSql);
		 bool InsertImage(const QString &strSql, QVariant& ImageData);
		 void run();
		 QString m_strError;
		 QString m_strSql;
		 DataStructDefine* m_TargetStruct;
		 static sqlPlugin::DBSelect select;
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
		static DataLib dataLib;
		QSqlDatabase m_dataBase;

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
	};
}

#endif
