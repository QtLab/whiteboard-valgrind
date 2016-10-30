#pragma once

#include <QGraphicsScene>

namespace Whiteboard {

class ProcessRunner;

class Scene : public QGraphicsScene
{
	Q_OBJECT
public:
	Scene(QObject* p = nullptr);

public slots:

	void openExecutable(const QString& path);
	void processUntilNextLine();

private:

	void processNextRecord(const QJsonObject& obj);

	ProcessRunner* runner_ = nullptr;
};

} // namespace Whiteboard

