#include "QueryAddDialog.h"

QueryAddDialog::QueryAddDialog(QWidget* parent)
	: QDialog(parent)
	, ui(new Ui::QueryAddDialog) 
{
	ui->setupUi(this);
}
QueryAddDialog::~QueryAddDialog()
{
	delete ui;
}

QString QueryAddDialog::GetQueryName() const
{
	return ui->QueryNameLine->text();
}
QString QueryAddDialog::GetQueryData() const
{
	return ui->QueryLine->toPlainText();
}

void QueryAddDialog::on_AcceptButton_clicked()
{
	accept();
}
void QueryAddDialog::on_CancelButton_clicked()
{
	reject();
}