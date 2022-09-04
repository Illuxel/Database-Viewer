#pragma once

#include <QObject>

#include "ui_SearchDialog.h"

namespace Ui 
{
	class SearchDialog;
}

class SearchDialog
	: public QDialog 
{
	Q_OBJECT

public:
	SearchDialog(QWidget* parent = nullptr);
	~SearchDialog() override;

private slots:
	void on_StartSearch_clicked();
	void on_CancelButton_clicked();

private:
	Ui::SearchDialog* ui;
};