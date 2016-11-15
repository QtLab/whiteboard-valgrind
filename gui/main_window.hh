#pragma once

#include <QMainWindow>

namespace Whiteboard {

class Scene;
class Debugger;
class SourceView;

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

	void resizeEvent(QResizeEvent* e) override;
	void showEvent(QShowEvent* e) override;

	Ui::MainWindow* ui_;
	Scene* scene_;
	Debugger* debugger_;
	SourceView* sourceView_;

	bool sceneTransofrmationSet_ = false;
};


} // namespace Whiteboard
