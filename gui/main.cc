#include "main_window.hh"

#include <QApplication>

#include <iostream>

int main(int argc, char** argv)
{
	QApplication app(argc, argv);

	Whiteboard::MainWindow mainWindow;

	if (argc >= 2)
		mainWindow.openExecutable(argv[2]);

	mainWindow.show();

	return app.exec();
}
