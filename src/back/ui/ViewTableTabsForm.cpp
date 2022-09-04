#include "ViewTableTabsForm.h"

ViewTableTabsForm::ViewTableTabsForm(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::ViewTableTabsForm())
{
	ui->setupUi(this);
}

ViewTableTabsForm::~ViewTableTabsForm()
{
	delete ui;
}
