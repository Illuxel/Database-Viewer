#include "SearchDialog.h"
#include "../Database.h"

#include <QLabel>
#include <QCheckBox>
#include <QMessageBox>
#include <QTableWidget>

SearchDialog::SearchDialog(QWidget* parent)
	: QDialog(parent)
	, ui(new Ui::SearchDialog)
{
	ui->setupUi(this);

	Database* db = Database::Instance();
	QVBoxLayout* CBLayout = ui->CheckBoxLayout;

	setWindowTitle(windowTitle().arg(db->GetCurrentTableName()));

	for (const auto& colum : db->GetCurrentTable().Columns) 
	{
		QCheckBox* box = new QCheckBox(colum);

		box->setChecked(false);
		CBLayout->addWidget(box);
	}
}

SearchDialog::~SearchDialog()
{
	delete ui;
}

void SearchDialog::on_StartSearch_clicked()
{
	Database* db = Database::Instance();
	QVBoxLayout* CBLayout = ui->CheckBoxLayout;

	QString SearchText = ui->SearchText->text();

	if (SearchText.isEmpty())
	{
		QMessageBox::warning(this, "Error", "Search line is empty!");
		return;
	}

	{
		QDialog* dialog = new QDialog(this);
		QTabWidget* tabWidget = new QTabWidget();

		dialog->setWindowTitle("Founded rows");
		dialog->setMinimumHeight(500);
		dialog->setMinimumWidth(450);

		dialog->setLayout(new QVBoxLayout());
		dialog->layout()->addWidget(tabWidget);

		for (int i = 0; i < CBLayout->count(); ++i)
		{
			QCheckBox* checkbox = static_cast<QCheckBox*>(CBLayout->itemAt(i)->widget());

			if (checkbox->isChecked())
			{
				QString boxName = checkbox->text();

				QString MatchWordTmpl = "SELECT * FROM %1 WHERE %2='%3';";
				QString SimilarWordTmpl = "SELECT * FROM %1 WHERE %2='%%3%';";

				QString query = ui->MatchWord->isChecked() ? MatchWordTmpl.arg(db->GetCurrentTableName()).arg(boxName).arg(SearchText)
					: SimilarWordTmpl.arg(db->GetCurrentTableName()).arg(boxName).arg(SearchText);

				Database::AddQuery(boxName, query);

				const auto& records = Database::ExecuteQueryName(boxName);

				Database::RemoveQuery(boxName);

				if (records.isEmpty())
					continue;

				QWidget* widget = new QWidget(tabWidget);
				QTableWidget* table = new QTableWidget();

				table->setRowCount(records.size());
				table->setColumnCount(records.isEmpty() ? 0 : records.first().count());

				table->setEditTriggers(QAbstractItemView::NoEditTriggers);

				QVBoxLayout* layout = new QVBoxLayout();

				layout->addWidget(table);
				widget->setLayout(layout);

				tabWidget->addTab(widget, boxName);

				// rows
				for (int i = 0; i < records.size(); ++i)
				{
					// columns
					for (int j = 0; j < records[i].count(); ++j)
					{
						QTableWidgetItem* item = new QTableWidgetItem();
						
						item->setData(Qt::DisplayRole, records[i].value(records[i].fieldName(j)));
						table->setItem(i, j, item);
					}
				}
			}
		}

		if (!tabWidget->count())
			QMessageBox::information(this, "Error", "There is no rows with value " + SearchText);
		else 
			dialog->exec();

		dialog->deleteLater();
	}

	accept();
}
void SearchDialog::on_CancelButton_clicked()
{
	reject();
}
