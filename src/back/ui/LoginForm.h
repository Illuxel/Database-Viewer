#pragma once

#include "ui_LoginForm.h"

#include "DBViewForm.h"

namespace Ui
{ 
	class LoginForm;
}

class LoginForm
	: public QWidget
{
	Q_OBJECT

public:
	LoginForm(QWidget* parent = nullptr);
	~LoginForm();

private slots:
	void on_ConnectDBButton_clicked();

	void on_ConnectServerButton_clicked();
	void on_DisconnectServerButton_clicked();

	void OnDatabaseConnected(Database::AccessType type);

	void OnServerConnected();
	void OnServerDisconnected(const QString& info);

	void OnViewDBEnded();

signals:
	void DatabaseSelected(const QString& dbName, const QJsonObject& user);

private:
	DBViewForm* m_DBviewForm = nullptr;

	Ui::LoginForm* ui;
};

