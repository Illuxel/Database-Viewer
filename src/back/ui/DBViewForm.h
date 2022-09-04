#pragma once

#include "ui_DBViewForm.h"

#include "../Database.h"

namespace Ui 
{
	class DBViewForm;
}

class QueryListForm;
class QueryAddDialog;

class QTableView;

class DBViewForm
	: public QMainWindow
{
	Q_OBJECT

public:
	DBViewForm(Database::AccessType type, QWidget* parent = nullptr);
	~DBViewForm();

private slots:
	void OnTabCurrentChanged(int index);

	void on_QueryList_triggered();
	void on_QueryAdd_triggered();

	void on_RefreshButton_triggered();
	void on_BackButton_triggered();

	void on_PrintButton_triggered();

	void on_SaveChangesButton_triggered();
	void on_DiscardChangesButton_triggered();

	void on_AddRowButton_clicked();
	void on_RemoveRowButton_clicked();
	void on_SearchButton_clicked();

signals:
	void ViewEnded();

private:
	QTableView* m_TableView = nullptr;
	QSqlTableModel* m_Model = nullptr;

	Ui::DBViewForm* ui = nullptr;

	QueryListForm* m_QueryList = nullptr;
};