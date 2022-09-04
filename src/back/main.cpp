#include <QApplication>

#include "../front/style/QDarkStyle/DarkStyle.h"

#include "ui/LoginForm.h"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);

	// style our application with custom dark style
	QApplication::setStyle(new DarkStyle);

	// create our mainwindow instance
	LoginForm form;
	form.show();

	return app.exec();
}