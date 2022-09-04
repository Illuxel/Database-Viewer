#pragma once

#include <QWidget>

#include "ui_ViewTableTabsForm.h"

namespace Ui 
{
	class ViewTableTabsForm;
};


class ViewTableTabsForm : public QWidget
{
	Q_OBJECT

public:
	ViewTableTabsForm(QWidget *parent = nullptr);
	~ViewTableTabsForm();



private:
	Ui::ViewTableTabsForm *ui;
};
