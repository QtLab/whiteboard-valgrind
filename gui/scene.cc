#include "scene.hh"

#include "stack_block_item.hh"

#include <QDebug>

namespace Whiteboard {

Scene::Scene(QObject* p) : QGraphicsScene(p)
{
}

void Scene::setViewportSize(const QSize& sz)
{
	setSceneRect(0, 0, sz.width(), sz.height());
}

void Scene::onStackChange(quint64 addr)
{
	if (!stack_)
	{
		stack_ = new StackBlockItem(addr);
		addItem(stack_);
		stack_->setPos(10, 10);
		stack_->show();
	}
}

void Scene::onMemEvent(const MemEvent& e)
{
	qDebug() << e;

	// stack?
	if (stack_ && stack_->ownsAddress(e.addr))
	{
		stack_->addMemEvent(e);
	}

}


} // namespace Whiteboard
