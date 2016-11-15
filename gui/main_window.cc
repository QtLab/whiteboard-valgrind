#include "main_window.hh"
#include "ui_main_window.h"

#include "scene.hh"
#include "debugger.hh"

#include <QTimer>

namespace Whiteboard {

MainWindow::MainWindow(QWidget *p) :
	QMainWindow(p),
	ui_(new Ui::MainWindow)
{
	ui_->setupUi(this);
	scene_ = new Scene(this);
	debugger_ = new Debugger(this);

	connect(debugger_, SIGNAL(statusChanged(const QString&)), statusBar(), SLOT(showMessage(const QString&)));
}

MainWindow::~MainWindow()
{
	delete ui_;
}

void MainWindow::openExecutable(const QString& file)
{
	QTimer::singleShot(100, [this, file]
	{
		debugger_->openExecutable(file);
	});
}

} // namespace Whiteboard
