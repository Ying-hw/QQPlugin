#include "stdafx1.h"
#include "SqlDefine.h"


DataLib::DataLib():m_Port(3306){

}

DataLib::~DataLib() {

}

bool DataLib::openDataLib() {
	m_strDataLibUserName = "ccroot";
	m_strPassWD = "root";
	m_strDataBaseName = "qq";
	m_strAddress = "192.168.1.35";
	m_dataBase = QSqlDatabase::addDatabase("QMYSQL");
	m_dataBase.setDatabaseName(m_strDataBaseName);
	m_dataBase.setPassword(m_strPassWD);
	m_dataBase.setHostName(m_strAddress);
	m_dataBase.setPort(m_Port);
	m_dataBase.setUserName(m_strDataLibUserName);
	return m_dataBase.open();
}

DataStructDefine& DBSelect::GetDataLib(const QString &strSql) {
	static DataStructDefine Data;
	Data.m_lstAllData.clear();
	Data.m_strError.clear();
	QSqlQuery query;
	query.prepare(strSql);
	Data.m_Result = query.exec();
	if (Data.m_Result)
		while (query.exec()) {
			QMap<QString, QVariant> rowMapData;
			for (int i = 0; i < query.record().count(); i++)
				rowMapData[query.record().fieldName(i)]
					= query.value(i);
			Data.m_lstAllData.append(rowMapData);
		}
	else
		Data.m_strError = query.lastError().text();
	return Data;
}

bool DBSelect::ExecuteSql(const QString &strSql) {
	QSqlQuery query;
	query.prepare(strSql);
	return query.exec();
}

DataStructDefine& DataStructDefine::operator = (const DataStructDefine& that) {
	if (&that != this) {
		this->m_lstAllData = that.m_lstAllData;
		this->m_Result = that.m_Result;
		this->m_strError = that.m_strError;
	}
	return *this;
}
