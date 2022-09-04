#include "Database.h"

#include <QSqlError>
#include <QFile>

Database::Database()
	: currentDB("Server") {}
Database::~Database() 
{
	Database::SaveQueries();
	Database::StopDatabase();
	Database::StopServer();
}

Database* Database::s_instance{ nullptr };
Database* Database::Instance()
{
	if (!s_instance) s_instance = new Database();
	return s_instance;
}

void Database::InitServer(const QJsonObject& connectionData)
{
	{
		Instance()->ConnectionStrJson = connectionData;

		QString qtdriver = connectionData["qtdriver"].toString();

		QString driver = connectionData["driver"].toString();
		QString server = connectionData["server"].toString();

		if (!QSqlDatabase::isDriverAvailable(qtdriver))
			return;

		auto user = connectionData["user"].toObject();

		QString name = user["name"].toString();
		QString pass = user["pass"].toString();

		Instance()->ConnectionStr = driver + server + name + pass;

		if (QSqlDatabase::contains("Server"))
			QSqlDatabase::removeDatabase("Server");

		QSqlDatabase DB = QSqlDatabase::addDatabase(qtdriver, "Server");
		DB.setDatabaseName(Instance()->ConnectionStr);
	}

	QSqlDatabase db = QSqlDatabase::database("Server");

	if (db.open())
	{
		Instance()->LoadDatabaseInfos();
		emit Instance()->ServerConnected();
	}
	else
	{
		QString error = db.lastError().driverText();
		qDebug() << error;

		emit Instance()->ServerDisconnected(error);
	}
}
void Database::InitDatabase(const QJsonObject& user)
{
	if (!QSqlDatabase::contains("Server"))
		return;

	Database* inst = Database::Instance();

	AccessType type = AccessType::Client;

	if (inst->ConnectionStrJson["admin"].toObject() == user)
	{
		type = AccessType::Admin;
	}

	QString database = inst->currentDB;

	{
		QString qtdriver = inst->ConnectionStrJson["qtdriver"].toString();
		QSqlDatabase db = QSqlDatabase::addDatabase(qtdriver, database);

		db.setDatabaseName(inst->ConnectionStr + "Database=" + database);
	}

	auto db = QSqlDatabase::database(database);

	if (!db.open())
	{
		QString error = db.lastError().driverText();
		qDebug() << error;
		return;
	}

	emit inst->DatabaseConnected(type);
}

void Database::StopServer()
{
	Instance()->currentDB = "Server";

	QSqlDatabase::database("Server").close();
	QSqlDatabase::removeDatabase("Server");

	Instance()->DBList.clear();

	emit Instance()->ServerDisconnected("Server connection stopped!");
}
void Database::StopDatabase()
{
	QString db = Instance()->currentDB;

	if (QSqlDatabase::contains(db))
	{
		QSqlDatabase::database(db).close();
		QSqlDatabase::removeDatabase(db);
	}
}

QString Database::GetCurrentDBName() const
{
	return currentDB;
}

QString Database::GetCurrentTableName() const
{
	return currentTable;
}
TableInfo Database::GetCurrentTable() const 
{
	for (const auto& table : GetTableList())
		if (table.Name == GetCurrentTableName())
			return table;
	return {};
}

QStringList Database::GetDatabaseList() const
{
	return QStringList(DBList.keys());
}

void Database::SetCurrentDB(const QString& db)
{
	currentDB = db;
}
void Database::SetCurrentTable(const QString& table)
{
	currentTable = table;
}

QList <TableInfo> Database::GetTableList() const
{
	return DBList.value(currentDB);
}
QList <QSqlRecord> Database::GetTableRecords() const
{
	return QueryExecute(QString("USE %1 SELECT * FROM %2;")
		.arg(currentDB)
		.arg(currentTable), currentDB);
}

QMap<QString, QList<QSqlRecord>> 
Database::SearchFromDB(const QString& text, const QStringList& columns)
{
	

	return {};
}

bool Database::LoadQueries()
{
	Database* db = Instance();

	QFile file("queries.json");

	if (!file.open(QFile::ReadOnly | QFile::Text))
	{
		return false;
	}

	QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
	db->SavedQueries = doc.object();

	return true;
}
bool Database::SaveQueries()
{
	Database* db = Instance();

	QFile file("queries.json");

	if (!file.open(QFile::WriteOnly)) 
	{
		return false;
	}

	QJsonDocument doc(db->SavedQueries);

	file.write(doc.toJson());
	file.close();

	return true;
}

QList <QSqlRecord> Database::ExecuteQueryName(const QString& name)
{
	Database* db = Instance();
	return db->QueryExecute(GetQueryData(name), db->currentDB);
}

QStringList Database::GetQueryNameList()
{
	Database* db = Instance();
	return db->SavedQueries[db->currentDB].toObject().keys();
}
QString Database::GetQueryData(const QString& name)
{
	Database* db = Instance();
	return db->SavedQueries[db->currentDB][name].toString();
}
/**
 *	{
 *		"db1": {
 *			"query1": "queryData",
 *			"query2": "queryData",
 *			"query3": "queryData"
 *		},
 *		"db2": {
 *			"query1": "queryData",
 *			"query2": "queryData",
 *			"query3": "queryData"
 *		}
 *	}
 */
void Database::AddQuery(const QString& queryName, const QString& query)
{
	Database* db = Instance();

	QString dbName = db->currentDB;

	if (!db->SavedQueries.contains(db->currentDB))
	{
		db->SavedQueries.insert(db->currentDB, QJsonObject({
			{ 
				queryName,
				query
			}
		}));
	}
	else {
		QJsonObject database(db->SavedQueries);
		QJsonObject queries(database[dbName].toObject());

		queries.insert(queryName, query);
		database.insert(dbName, queries);

		db->SavedQueries = database;
	}
}
void Database::RemoveQuery(const QString& name)
{
	Database* db = Instance();

	QString dbName = db->currentDB;

	QJsonObject database(db->SavedQueries);
	QJsonObject queries(database[dbName].toObject());

	queries.remove(name);
	database[dbName] = queries;

	db->SavedQueries = database;
}

QSqlTableModel* Database::GetTableModel() const
{
	QSqlDatabase DB = QSqlDatabase::database(currentDB);
	QSqlTableModel* Model = new QSqlTableModel(nullptr, DB);

	Model->setTable(currentTable);
	Model->setEditStrategy(QSqlTableModel::OnManualSubmit);
	Model->select();

	return Model;
}

void Database::LoadDatabaseInfos()
{
	{
		QString SelectDatabases = "SELECT name FROM sys.databases WHERE database_id NOT BETWEEN 1 AND 4;"; // Selects all db names

		for (const auto& record : QueryExecute(SelectDatabases, currentDB))
		{
			QString dbName = record.value("name").toString();
			DBList.insert(dbName, {});
		}
	}

	{
		QString SelectTables = "USE %1 SELECT TABLE_NAME FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_NAME NOT IN(\'sysdiagrams\');";
		QString SelectTableColumns = "USE %1 SELECT COLUMN_NAME FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_NAME = \'%2\';";
		/**
		 *	Accessing db names
		 */
		for (const auto& DBname : DBList.keys())
		{
			/**
			 *	Inserting table names and column names
			 */
			for (const auto& record : QueryExecute(SelectTables.arg(DBname), currentDB))
			{
				TableInfo table;

				table.Name = record.value("TABLE_NAME").toString();

				{
					const auto& records = QueryExecute(SelectTableColumns.arg(DBname).arg(table.Name), currentDB);
					for (const auto& record : records)
						table.Columns.push_back(record.value("COLUMN_NAME").toString());
				}

				auto& tables = DBList[DBname];
				tables.push_back(table);
			}
		}
	}

	Database::LoadQueries();
}

QList <QSqlRecord> Database::QueryExecute(const QString& query, const QString& dbName) const
{
	QSqlDatabase db = QSqlDatabase::database(dbName);
	QSqlQuery Query(db);

	bool is_exec = Query.exec(query);

	if (!is_exec)
	{
		qDebug() << Query.lastError().text();
		return {};
	}

	QList <QSqlRecord> temp;

	while (Query.next())
	{
		temp.push_back(Query.record());
	}

	return temp;
}
