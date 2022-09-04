#include "QueryListForm.h"
#include "../Database.h"

#include <QDialog>
#include <QTableWidget>

QueryListForm::QueryListForm(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::QueryListForm)
{
	ui->setupUi(this);

	Database* db = Database::Instance();

	setWindowTitle(windowTitle().arg(db->GetCurrentTableName()));

	QStringList list = Database::GetQueryNameList();
	ui->QueryList->addItems(list);

	ui->QueryList->setCurrentRow(-1);
	ui->QueryList->clearSelection();
}
QueryListForm::~QueryListForm()
{
	delete ui;
}

void QueryListForm::on_QueryList_itemClicked(QListWidgetItem* item)
{
	QString query = Database::GetQueryData(item->text());
	ui->QueryText->setText(query);
}
void QueryListForm::on_ExecuteQueryButton_clicked()
{
	QString currentItem = ui->QueryList->currentItem()->text();

	QDialog* dialog = new QDialog(this);

	dialog->setWindowTitle("Executed query");
	dialog->setMinimumHeight(300);
	dialog->setMinimumWidth(300);

	QVBoxLayout* layout = new QVBoxLayout();
	QTableWidget* table = new QTableWidget();

	table->setEditTriggers(QAbstractItemView::NoEditTriggers);

	{
		const QList<QSqlRecord>& records = Database::ExecuteQueryName(currentItem);

		table->setRowCount(records.size());
		table->setColumnCount(records.isEmpty() ? 0 : records.first().count());

		for (int i = 0; i < records.size(); ++i)
		{
			for (int j = 0; j < records[i].count(); ++j)
			{
				QTableWidgetItem* item = new QTableWidgetItem();

				item->setData(Qt::DisplayRole, records[i].value(records[i].fieldName(j)));
				table->setItem(i, j, item);
			}
		}
	}

	layout->addWidget(table);
	dialog->setLayout(layout);

	dialog->exec();

	delete dialog;
}
void QueryListForm::on_RemoveQueryButton_clicked()
{
	QString item = ui->QueryList->currentItem()->text();

	Database::RemoveQuery(item);
	Database::SaveQueries();

	ui->QueryList->removeItemWidget(ui->QueryList->currentItem());
	ui->QueryText->clear();
}
