#pragma once

#include <QSqlDatabase>

#include <QTableView>
#include <QSqlTableModel>

#include <QSqlQuery>
#include <QSqlRecord>

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

struct TableInfo
{
	QString Name;
	QStringList Columns;
};

class Database
	: public QObject
{
	Q_OBJECT

	Database();

public:
	~Database();

	enum AccessType 
	{
		Admin,
		Client
	};
	Q_ENUM(AccessType)

	static Database* Instance();

	QStringList	GetDatabaseList() const;

	void SetCurrentDB(const QString& db);
	void SetCurrentTable(const QString& table);

	QString		GetCurrentDBName() const;
	QString		GetCurrentTableName() const;
	TableInfo	GetCurrentTable() const;

	QList <TableInfo>  GetTableList() const;
	QSqlTableModel*    GetTableModel() const;
	QList <QSqlRecord> GetTableRecords() const;

	static QMap <QString, QList <QSqlRecord>> SearchFromDB(const QString& text, const QStringList& columns);

	static bool LoadQueries();
	static bool SaveQueries();

	static void AddQuery(const QString& name, const QString& query);
	static void RemoveQuery(const QString& name);

	static QList <QSqlRecord> ExecuteQueryName(const QString& name);

	static QStringList	GetQueryNameList();
	static QString		GetQueryData(const QString& name);

	Database(const Database&) = delete;
	Database(Database&&) = delete;
	void operator=(const Database&) = delete;

private:
	void LoadDatabaseInfos();

	QList <QSqlRecord> QueryExecute(const QString& query, const QString& db) const;

signals:
	void ServerConnected();
	void ServerDisconnected(const QString& reason);

	void DatabaseConnected(AccessType type);

public slots:
	/**
	 *  Starts server connection to get database list
	 */
	static void InitServer(const QJsonObject& connectionData);
	/**
	 *  Connects to selected database
	 */
	static void InitDatabase(const QJsonObject& user);
	/**
	 *  Stops current database connection
	 */
	static void StopServer();
	/**
	 *  Stops current database connection
	 */
	static void StopDatabase();

private:
	static Database* s_instance;

	QString currentDB, currentTable;
	QHash <QString, QList <TableInfo>> DBList;

	QString ConnectionStr;
	QJsonObject ConnectionStrJson, SavedQueries;
};

