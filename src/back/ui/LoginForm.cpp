#include "LoginForm.h"
#include "../Database.h"

#include <QHostInfo>
#include <QMessageBox>

LoginForm::LoginForm(QWidget* parent)
	: QWidget(parent)
	, ui(new Ui::LoginForm)
{
	ui->setupUi(this);

	Database* db = Database::Instance();

	connect(db, &Database::ServerConnected, this, &LoginForm::OnServerConnected);
	connect(db, &Database::ServerDisconnected, this, &LoginForm::OnServerDisconnected);
			
	connect(db, &Database::DatabaseConnected, this, &LoginForm::OnDatabaseConnected);
}
LoginForm::~LoginForm()
{
	delete ui;
}

void LoginForm::on_ConnectDBButton_clicked()
{
	Database* db = Database::Instance();
	db->SetCurrentDB(ui->DatabaseList->currentText());

	QString login = ui->UsernameLine->text();
	QString pass = ui->PasswordLine->text();

	if (login.isEmpty() || pass.isEmpty())
	{
		QMessageBox::warning(this, "Error", "User name or password shouldn't be empty!");
		return;
	}

	QJsonObject user
	{
		{ "login", login },
		{ "pass", pass }
	};

	Database::InitDatabase(user);
}

void LoginForm::on_ConnectServerButton_clicked()
{
	QJsonObject connectionData
	{
		{ "qtdriver", "QODBC"},
		{ "driver", QString("DRIVER={%1};")	.arg(ui->DriverList->currentText()) },
		{ "server", QString("SERVER=%1;")	.arg(ui->ServerNameLine->text()) },
		{ "user",	QJsonObject({

			{ "name", QString("Uid=%1;")	.arg(QHostInfo::localDomainName()) },
			{ "pass", QString("Pwd=%1;")	.arg("")}})

		},
		{ "admin", QJsonObject({

			{ "login", "admin" },
			{ "pass", "1234" }})
			
		}
	};

	Database::InitServer(connectionData);
}
void LoginForm::on_DisconnectServerButton_clicked()
{
	Database::StopServer();
}

void LoginForm::OnDatabaseConnected(Database::AccessType type)
{
	m_DBviewForm = new DBViewForm(type);
	connect(m_DBviewForm, &DBViewForm::ViewEnded, this, &LoginForm::OnViewDBEnded);
	m_DBviewForm->show();

	hide();
}

void LoginForm::OnServerConnected()
{
	ui->ConnectServerButton->setEnabled(false);
	ui->DisconnectServerButton->setEnabled(true);

	ui->UsernameLine->setEnabled(true);
	ui->PasswordLine->setEnabled(true);

	ui->ConnectDBButton->setEnabled(true);

	Database* temp = Database::Instance();
	ui->DatabaseList->addItems(temp->GetDatabaseList());
}
void LoginForm::OnServerDisconnected(const QString& info)
{
	ui->DatabaseList->clear();

	ui->ConnectServerButton->setEnabled(true);
	ui->DisconnectServerButton->setEnabled(false);

	ui->UsernameLine->setEnabled(false);
	ui->PasswordLine->setEnabled(false);

	ui->ConnectDBButton->setEnabled(false);

	QMessageBox::information(this, "Info", info);
}

void LoginForm::OnViewDBEnded()
{
	m_DBviewForm->deleteLater();
	show();
}
