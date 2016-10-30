#include "main_window.hh"
#include "ui_main_window.h"

#include "scene.hh"

namespace Whiteboard {

MainWindow::MainWindow(QWidget *p) :
	QMainWindow(p),
	ui_(new Ui::MainWindow)
{
	ui_->setupUi(this);
	scene_ = new Scene(this);
}

MainWindow::~MainWindow()
{
	delete ui_;
}

void MainWindow::openExecutable(const QString& file)
{
	scene_->openExecutable(file);
}

} // namespace Whiteboard
