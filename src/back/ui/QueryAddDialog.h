#pragma once

#include "ui_QueryAddDialog.h"

#include <QDialog>

namespace Ui
{
	class QueryAddDialog;
}

class QueryAddDialog :
    public QDialog
{
	Q_OBJECT

public:
	QueryAddDialog(QWidget* parent = nullptr);
	~QueryAddDialog();

	QString GetQueryName() const;
	QString GetQueryData() const;

private slots:
	void on_AcceptButton_clicked();
	void on_CancelButton_clicked();

private:
	Ui::QueryAddDialog* ui;
};

