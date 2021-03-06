#include "sqlPlugin.h"

sqlPlugin::DataLib::DataLib() :m_Port(3306), m_IsOpen(false), hostIsError(false) {

}


sqlPlugin::DataLib::~DataLib() {
	wait();
	if (m_dataBase.isOpen())
		m_dataBase.close();
}

void sqlPlugin::DataLib::openDataLib(QHostInfo host) {
	if (host.error() == QHostInfo::NoError) {
		hostIsError = false;
		if (!m_dataBase.isOpen() && !isRunning()) {
			m_strDataLibUserName = "root";
			m_strPassWD = "root";
			m_strDataBaseName = "qq";
			m_strAddress = host.addresses().first().toString();
			m_dataBase = QSqlDatabase::addDatabase("QMYSQL");
			m_dataBase.setDatabaseName(m_strDataBaseName);
			m_dataBase.setPassword(m_strPassWD);
			m_dataBase.setHostName(m_strAddress);
			m_dataBase.setPort(m_Port);
			m_dataBase.setUserName(m_strDataLibUserName);
			start();
		}
	}
	else {
		m_strError = host.errorString();
		hostIsError = true;
		start();
	}
}


void sqlPlugin::DataLib::ConnectServer(QString strAddr, quint16 port)
{
	m_Port = port;
	QHostInfo::lookupHost(strAddr, this, SLOT(openDataLib(QHostInfo)));
}

bool sqlPlugin::DataLib::GetOpenResult()
{
	return m_IsOpen;
}

sqlPlugin::DataLib* sqlPlugin::DataLib::GetDataLibInstance()
{
	return &dataLib;
}
sqlPlugin::DBSelect SQLPLUGIN_EXPORT sqlPlugin::DBSelect::select;
sqlPlugin::DataLib SQLPLUGIN_EXPORT sqlPlugin::DataLib::dataLib;
sqlPlugin::DBSelect* sqlPlugin::DataLib::GetSelectInstance()
{
	return &DBSelect::select;
}


void sqlPlugin::DataLib::run()
{
	if (hostIsError)
		return;

	if (!m_dataBase.open()) {
		m_strError = m_dataBase.lastError().text();
		m_IsOpen = false;
	}
	else 
		m_IsOpen = true;
}

sqlPlugin::DBSelect::DBSelect(QObject* parent /*= 0*/) : QThread(parent), m_TargetStruct(NULL)
{

}

sqlPlugin::DBSelect::~DBSelect()
{

}

void sqlPlugin::DBSelect::GetData(DataStructDefine& sourceData, const QString &strSql) {
	sourceData.m_lstAllData.clear();
	sourceData.m_strError.clear();
	wait();
	m_strSql = strSql;
	m_TargetStruct = &sourceData;
	start();
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

void sqlPlugin::DBSelect::run()
{
	QSqlQuery query(DataLib::dataLib.m_dataBase);
	query.prepare(m_strSql);
	m_TargetStruct->m_Result = query.exec();
	if (m_TargetStruct->m_Result) 
		while (query.next()) {
			QMap<QString, QVariant> rowMapData;
			for (int i = 0; i < query.record().count(); i++)
				rowMapData[query.record().fieldName(i)] = query.value(i);
			m_TargetStruct->m_lstAllData.append(rowMapData);
		}
	else {
		if (query.lastError().type() == QSqlError::ConnectionError)
			sqlPlugin::DataLib::GetDataLibInstance()->start();
		m_strError = query.lastError().text();
	}
}

const sqlPlugin::DataStructDefine& sqlPlugin::DataStructDefine::operator= (const DataStructDefine& that) {
	DataLib::GetDataLibInstance()->GetSelectInstance()->wait();
	if (&that != this) {
		this->m_lstAllData = that.m_lstAllData;
		this->m_Result = that.m_Result;
		this->m_strError = that.m_strError;
	}
	return *this;
}

sqlPlugin::DataStructDefine::DataStructDefine(const DataStructDefine& target)
{
	DataLib::GetDataLibInstance()->GetSelectInstance()->wait();
	if (&target != this) {
		this->m_lstAllData = target.m_lstAllData;
		this->m_Result = target.m_Result;
		this->m_strError = target.m_strError;
	}
}

sqlPlugin::DataStructDefine::DataStructDefine()
{

}
