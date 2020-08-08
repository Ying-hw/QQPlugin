#include "sqlPlugin.h"

sqlPlugin::DataLib::DataLib() :m_Port(3306), m_IsOpen(false) {

}


sqlPlugin::DataLib::~DataLib() {
	if (m_dataBase.isOpen())
		m_dataBase.close();
}

void sqlPlugin::DataLib::openDataLib() {
	if (!m_dataBase.isOpen()) {
		m_strDataLibUserName = "root";
		m_strPassWD = "root";
		m_strDataBaseName = "qq";
		m_strAddress = "192.168.1.48";
		m_dataBase = QSqlDatabase::addDatabase("QMYSQL");
		m_dataBase.setDatabaseName(m_strDataBaseName);
		m_dataBase.setPassword(m_strPassWD);
		m_dataBase.setHostName(m_strAddress);  
		m_dataBase.setPort(m_Port);
		m_dataBase.setUserName(m_strDataLibUserName);
		start();
	}
}

bool sqlPlugin::DataLib::GetOpenResult()
{
	return m_IsOpen;
}

sqlPlugin::DataLib* sqlPlugin::DataLib::GetDataLibInstance()
{
	static DataLib dataLib;
	return &dataLib;
}

sqlPlugin::DBSelect* sqlPlugin::DataLib::GetSelectInstance()
{
	static sqlPlugin::DBSelect select;
	return &select;
}


void sqlPlugin::DataLib::run()
{
	if (!m_dataBase.open()) {
		m_strError = m_dataBase.lastError().text();
		m_IsOpen = false;
	}
	else {
		m_IsOpen = true;
	}
}

sqlPlugin::DataStructDefine& sqlPlugin::DBSelect::GetData(const QString &strSql) {
	static sqlPlugin::DataStructDefine Data;
	Data.m_lstAllData.clear();
	Data.m_strError.clear();
	QSqlQuery query;
	query.prepare(strSql);
	Data.m_Result = query.exec();
	if (Data.m_Result)
		while (query.next()) {
			QMap<QString, QVariant> rowMapData;
			for (int i = 0; i < query.record().count(); i++)
				rowMapData[query.record().fieldName(i)] = query.value(i);
			Data.m_lstAllData.append(rowMapData);
		}
	else {
		if (query.lastError().type() == QSqlError::ConnectionError)
			sqlPlugin::DataLib::GetDataLibInstance()->start();
		m_strError = query.lastError().text();
	}
	return Data;
}

bool sqlPlugin::DBSelect::ExecuteSql(const QString &strSql) {
	QSqlQuery query;
	query.prepare(strSql);
	if (!query.exec()) {
		m_strError = query.lastError().text();
		if (query.lastError().type() == QSqlError::ConnectionError)
			sqlPlugin::DataLib::GetDataLibInstance()->start();
		return false;
	}
	return true;
}

bool sqlPlugin::DBSelect::InsertImage(const QString &strSql, QVariant& ImageData)
{
	QSqlQuery query;
	query.prepare(strSql);
	query.addBindValue(ImageData);
	if (!query.exec()) {
		m_strError = query.lastError().text();
		if (query.lastError().type() == QSqlError::ConnectionError)
			sqlPlugin::DataLib::GetDataLibInstance()->start();
		return false;
	}
	return true;
}

const sqlPlugin::DataStructDefine& sqlPlugin::DataStructDefine::operator= (const DataStructDefine& that) {
	if (&that != this) {
		this->m_lstAllData = that.m_lstAllData;
		this->m_Result = that.m_Result;
		this->m_strError = that.m_strError;
	}
	return *this;
}

sqlPlugin::DataStructDefine::DataStructDefine(const DataStructDefine& target)
{
	if (&target != this) {
		this->m_lstAllData = target.m_lstAllData;
		this->m_Result = target.m_Result;
		this->m_strError = target.m_strError;
	}
}

sqlPlugin::DataStructDefine::DataStructDefine()
{

}

