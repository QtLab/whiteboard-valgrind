#include "scene.hh"

#include "memory_block_item.hh"

#include <QDebug>

namespace Whiteboard {

static const quint64 STACK_MARGIN = 128; // added at the end of the stack

Scene::Scene(QObject* p) : QGraphicsScene(p)
{
}

void Scene::setViewportSize(const QSize& sz)
{
	setSceneRect(0, 0, sz.width(), sz.height());
}

void Scene::onStackChange(quint64 addr)
{
	//qDebug() << "stack: " << addr;
	if (!stack_)
	{
		stack_ = new MemoryBlockItem(addr - STACK_MARGIN, STACK_MARGIN*2);
		stack_->setColor(Qt::gray);
		stack_->setName("Stack");
		addItem(stack_);
		stack_->setPos(10, 10);
		stack_->show();
	}
	else
	{
		stack_->moveAddr(addr - STACK_MARGIN);
	}
}

void Scene::onMemEvent(const MemEvent& e)
{
	// stack?
	if (stack_ && stack_->ownsAddress(e.addr))
	{
		stack_->addMemEvent(e);
	}
	else
	{
		qDebug() << "unknonw memory event: " << e;
	}

}


} // namespace Whiteboard
