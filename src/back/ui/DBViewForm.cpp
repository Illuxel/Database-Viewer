#include "DBViewForm.h"
#include "../Database.h"

#include <QVBoxLayout>
#include <QMessageBox>

#include <QPrinter>
#include <QPrintDialog>
#include <QTextDocument>

#include <QVariant>

#include "QueryListForm.h"
#include "QueryAddDialog.h"

#include "SearchDialog.h"

DBViewForm::DBViewForm(Database::AccessType type, QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::DBViewForm)
	, m_TableView(new QTableView)
{
	ui->setupUi(this);

	Database* db = Database::Instance();

	{
		QString titleName = windowTitle()
			.arg(db->GetCurrentDBName())
			.arg(QVariant::fromValue(type).toString());

		setWindowTitle(titleName);
	}

	if (type == Database::Admin)
	{
		ui->AddRowButton->setEnabled(true);
		ui->RemoveRowButton->setEnabled(true);
		ui->SaveChangesButton->setEnabled(true);
	}

	QList <TableInfo> TableList = db->GetTableList();

	if (TableList.isEmpty()) 
	{
		QMessageBox::critical(this, "Error", "This database is empty!");

		ui->menuQueries->setEnabled(false);
		ui->menuSave->setEnabled(false);
		ui->RefreshButton->setEnabled(false);

		return;
	}

	for (auto table = TableList.begin(); table != TableList.end(); ++table)
	{
		QWidget* widget = new QWidget(ui->TableTabsWidget);
		widget->setLayout(new QVBoxLayout(widget));

		ui->TableTabsWidget->addTab(widget, table->Name);
	}

	on_RefreshButton_triggered();
	m_TableView->show();
	connect(ui->TableTabsWidget, &QTabWidget::currentChanged, this, &DBViewForm::OnTabCurrentChanged);
}
DBViewForm::~DBViewForm()
{
	delete ui;

	m_TableView->deleteLater();
	m_QueryList->deleteLater();
}

void DBViewForm::on_QueryList_triggered()
{
	if (m_QueryList)
		delete m_QueryList;

	m_QueryList = new QueryListForm();
	m_QueryList->show();
}
void DBViewForm::on_QueryAdd_triggered()
{
	QueryAddDialog* dialog = new QueryAddDialog(this);

	if (dialog->exec() == QDialog::Accepted)
	{
		Database::AddQuery(dialog->GetQueryName(), dialog->GetQueryData());
		Database::SaveQueries();
	}

	dialog->deleteLater();
}

void DBViewForm::on_RefreshButton_triggered()
{
	Database* db = Database::Instance();

	if (m_Model) 
		delete m_Model;

	int index = ui->TableTabsWidget->currentIndex();
	QString text = ui->TableTabsWidget->tabText(index);

	db->SetCurrentTable(text);

	m_Model = db->GetTableModel();

	m_TableView->setModel(m_Model);

	QWidget* widget = ui->TableTabsWidget->widget(index);
	QLayout* layout = widget->layout();

	layout->addWidget(m_TableView);
}
void DBViewForm::on_BackButton_triggered()
{
	if (m_Model) {
		delete m_Model;
		m_Model = nullptr;
	}
	Database::StopDatabase();
	emit ViewEnded();
}

void DBViewForm::on_PrintButton_triggered()
{
	if (ui->SaveChangesButton->isEnabled())
		on_SaveChangesButton_triggered();

	QString strStream;
	QTextStream out(&strStream);

	const int rowCount = m_TableView->model()->rowCount();
	const int columnCount = m_TableView->model()->columnCount();

	out << "<html>\n"
		"<head>\n"
		"<meta Content=\"Text/html; charset=Windows-1251\">\n"
		<< QString("<title>%1</title>\n").arg(Database::Instance()->GetCurrentTableName())
		<< "</head>\n"
		"<body bgcolor=#ffffff link=#5000A0>\n"
		"<table border=1 cellspacing=0 cellpadding=2>\n";

	// headers
	out << "<thead><tr bgcolor=#f0f0f0>";
	for (int column = 0; column < columnCount; column++)
		if (!m_TableView->isColumnHidden(column))
			out << QString("<th>%1</th>").arg(m_TableView->model()->headerData(column, Qt::Horizontal).toString());
	out << "</tr></thead>\n";

	// data table
	for (int row = 0; row < rowCount; ++row) {
		out << "<tr>";
		for (int column = 0; column < columnCount; column++) {
			if (!m_TableView->isColumnHidden(column)) {
				QString data = m_TableView->model()->data(m_TableView->model()->index(row, column)).toString().simplified();
				out << QString("<td bkcolor=0>%1</td>").arg((!data.isEmpty()) ? data : QString("&nbsp;"));
			}
		}
		out << "</tr>\n";
	}
	out << "</table>\n"
		"</body>\n"
		"</html>\n";

	QTextDocument* document = new QTextDocument();
	document->setHtml(strStream);

	QPrinter printer;

	QPrintDialog* dialog = new QPrintDialog(&printer, this);
	if (dialog->exec() == QDialog::Accepted) 
	{
		document->print(&printer);
	}

	delete document;
}

void DBViewForm::on_SaveChangesButton_triggered()
{
	m_Model->submitAll();
	ui->connectionStatus->showMessage("Changes saved to " + Database::Instance()->GetCurrentTableName());
}
void DBViewForm::on_DiscardChangesButton_triggered()
{
	m_Model->revertAll();
	ui->connectionStatus->showMessage("Changes discarded to " + Database::Instance()->GetCurrentTableName());
}

void DBViewForm::on_AddRowButton_clicked()
{
	m_Model->insertRow(m_Model->rowCount(QModelIndex()));
}
void DBViewForm::on_RemoveRowButton_clicked()
{
	int rowPos = m_TableView->currentIndex().row();

	QMessageBox::StandardButton button = QMessageBox::warning(this, 
		"Deleting", 
		"Are you sure you want to delete row " + QString::number(rowPos) + '?',
		QMessageBox::Ok | QMessageBox::Cancel
	);

	if (QMessageBox::Cancel == button)
		return;
	
	m_Model->removeRow(rowPos);
}
void DBViewForm::on_SearchButton_clicked()
{
	SearchDialog* dialog = new SearchDialog(this);

	if (dialog->exec() == QDialog::Rejected)
		return;



	delete dialog;
}

void DBViewForm::OnTabCurrentChanged(int index)
{
	QString tableName = ui->TableTabsWidget->tabText(index);
	Database::Instance()->SetCurrentTable(tableName);

	on_RefreshButton_triggered();
}