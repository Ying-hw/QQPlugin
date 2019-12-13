#ifndef SQLDEFINE
#define SQLDEFINE

#include <QSqlDataBase>
#define SELECT_USER "SELECT * FROM user_define WHERE USER_NUMBER = '%1';"
#define SELECT_ "SELECT 1"

struct DataStructDefine;

struct DataLib{
	DataLib();
	~DataLib();
	quint32 m_Port;
	QString m_strDataBaseName;
	QString m_strAddress;
	QString m_strPassWD;
	QString m_strDataLibUserName;
	QSqlDatabase m_dataBase;
	bool openDataLib();
};

struct DBSelect{
	static DataStructDefine& GetDataLib(const QString &strSql);
	static bool ExecuteSql(const QString &strSql);
};

struct DataStructDefine{
	QList<QMap<QString, QVariant>> m_lstAllData;
	QString m_strError;
	bool m_Result;
	DataStructDefine& operator=(const DataStructDefine& that);
};

struct UserIdentify{
	QString strPassWd;
	QString UserName;
	QString UserNumber; 
};

#endif  //SQLDEFINE