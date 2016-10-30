#include "scene.hh"

#include "process_runner.hh"

#include <QJsonObject>

#include <QJsonDocument> //TODO debug
#include <QDebug>

namespace Whiteboard {

Scene::Scene(QObject* p) : QGraphicsScene(p)
{
	runner_ = new ProcessRunner(this);
}

void Scene::openExecutable(const QString& path)
{
	runner_->startExecutable(path);

	processUntilNextLine();
}

void Scene::processUntilNextLine()
{
	while(true)
	{
		QJsonObject obj = runner_->getNextRecord();
		processNextRecord(obj);
		QString action = obj["action"].toString();
		Q_ASSERT(!action.isEmpty());
		if (action == "line-step")
			break;
	}
}

void Scene::processNextRecord(const QJsonObject& obj)
{
	// TODO
	qDebug() << QJsonDocument(obj).toJson();
}

} // namespace Whiteboard
