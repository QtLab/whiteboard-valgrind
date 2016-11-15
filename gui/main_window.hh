#pragma once

#include <QMainWindow>

namespace Whiteboard {

class Scene;
class Debugger;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget* p = nullptr);
	~MainWindow();

public slots:

	void openExecutable(const QString& file);

private:
	Ui::MainWindow* ui_;
	Scene* scene_;
	Debugger* debugger_;


};


} // namespace Whiteboard
