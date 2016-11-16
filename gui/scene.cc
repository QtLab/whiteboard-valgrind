#include "scene.hh"

#include "memory_block_item.hh"

#include <QDebug>
#include <QGraphicsRectItem>

namespace Whiteboard {

static const quint64 STACK_MARGIN = 128; // added at the end of the stack

Scene::Scene(QObject* p) : QGraphicsScene(p)
{
	// inflate scene rect
	QGraphicsRectItem* strut = new QGraphicsRectItem(QRectF(0, 0, 2048, 2048));
	strut->show();
	addItem(strut);
}

void Scene::setViewportSize(const QSize& sz)
{
	// more trouble than it's worth
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
		foreach(MemoryBlockItem* block, heap_)
		{
			if (block->ownsAddress(e.addr))
			{
				block->addMemEvent(e);
				return;
			}
		}
		qDebug() << "unknown memory event: " << e;
	}

}

void Scene::onHeapEvent(const HeapEvent& e)
{
	if (e.type == HeapEvent::ALLOC)
	{
		if (heap_.contains(e.addr))
			throw std::runtime_error("Duplicate mem block");

		MemoryBlockItem* block = new MemoryBlockItem(e.addr, e.size);
		block->setName(e.call);
		block->setColor(Qt::cyan);
		block->setPos(10 + (10+8*30)*(heap_.size()+1), 10);
		addItem(block);

		heap_.insert(e.addr, block);
	}
	else
	{
		auto it = heap_.find(e.addr);
		if (it == heap_.end())
			throw std::runtime_error("Free on unknown block");

		removeItem(*it);
		delete *it;
		heap_.erase(it);
	}
}


} // namespace Whiteboard
