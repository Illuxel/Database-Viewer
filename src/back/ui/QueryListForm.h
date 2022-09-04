#pragma once

#include <QWidget>
#include "ui_QueryListForm.h"

namespace Ui 
{ 
	class QueryListForm; 
};

class QueryListForm : public QWidget
{
	Q_OBJECT

public:
	QueryListForm(QWidget *parent = nullptr);
	~QueryListForm();

private slots:
	void on_QueryList_itemClicked(QListWidgetItem* item);

	void on_ExecuteQueryButton_clicked();
	void on_RemoveQueryButton_clicked();

private:
	Ui::QueryListForm* ui;
};