#include "scene.hh"

#include <QDebug>

namespace Whiteboard {

Scene::Scene(QObject* p) : QGraphicsScene(p)
{
}

void Scene::onStackChange(quint64 addr)
{
	qDebug() << "stack change: " << addr;
}

void Scene::onMemEvent(const MemEvent& e)
{
	// TODO
	qDebug() << e;
}


} // namespace Whiteboard
