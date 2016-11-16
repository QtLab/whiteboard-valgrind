#include "scene.hh"

#include "memory_block_item.hh"

#include <QDebug>
#include <QGraphicsRectItem>

#include <algorithm>

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
	viewportSize_ = sz;
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
		//qDebug() << "unknown memory event: " << e;
	}

}

void Scene::onHeapEvent(const HeapEvent& e)
{
	if (e.type == HeapEvent::ALLOC)
	{
		qDebug() << " ALLOC " << e.addr;

		if (heap_.contains(e.addr))
			throw std::runtime_error("Duplicate mem block");

		MemoryBlockItem* block = new MemoryBlockItem(e.addr, e.size);
		block->setName(e.call);
		block->setColor(Qt::cyan);

		QPointF pos = findPlaceForBlock(block->boundingRect().size());
		block->setPos(pos);

		addItem(block);

		heap_.insert(e.addr, block);
	}
	else
	{
		qDebug() << " FREE " << e.addr;
		auto it = heap_.find(e.addr);
		if (it == heap_.end())
			throw std::runtime_error("Free on unknown block");

		removeItem(*it);
		delete *it;
		heap_.erase(it);
	}
}

QPointF Scene::findPlaceForBlock(const QSizeF& sz) const
{
	double x0 = 180;
	double y0 = 10;

	double xIncr = 45;
	double yIncr = 40;

	double maxX = 2048;
	double maxY = viewportSize_.height() - yIncr;

	for(double x = x0; x < maxX; x += xIncr)
	{
		for(double y = y0; y < maxY; y += yIncr)
		{
			QPointF candidate(x, y);
			QRectF rect(candidate, sz);
			auto list = items(candidate, Qt::IntersectsItemBoundingRect);
			// make sure there is no collision with another blocks
			if (std::count_if(
					list.begin(), list.end(),
					[](const QGraphicsItem* i) { return i->type() == int(ItemType::MEMORY_BLOCK); })
				== 0)
			{
				return candidate;
			}
		}
	}

	// nothing.... fall back to random
	int xr = qrand() % viewportSize_.width();
	int yr = qrand() % int(maxY);

	return QPointF(xr, yr);
}



} // namespace Whiteboard
