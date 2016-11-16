#include "memory_block_item.hh"

#include "mem_cell_item.hh"

#include <QPainter>

namespace Whiteboard {

static const int MARGIN = 3;
static const int HEADER_HEIGHT = 20;

MemoryBlockItem::MemoryBlockItem(quint64 addr, quint64 size, QGraphicsItem* p)
	: QGraphicsItem(p), addr_(addr), size_(size)
{
}

QRectF MemoryBlockItem::boundingRect() const
{
	double h = std::ceil(size_ / 8.0) * CELL_SIZE + MARGIN*2+HEADER_HEIGHT;
	double w = MARGIN*2 + CELL_SIZE*8;
	return QRectF(0, 0, w, h);
}

void MemoryBlockItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	// background
	double h = std::ceil(size_ / 8.0) * CELL_SIZE + MARGIN*2;
	double w = MARGIN*2 + CELL_SIZE*8;

	// body
	painter->fillRect(QRectF(0, HEADER_HEIGHT, w, h), color_);

	// header
	QBrush headerBrush(color_);
	headerBrush.setStyle(Qt::BDiagPattern);
	painter->fillRect(QRectF(0, 0, w, HEADER_HEIGHT), headerBrush);

	// caption
	painter->drawText(QRectF(MARGIN, MARGIN, w-MARGIN*2, HEADER_HEIGHT-MARGIN*2), Qt::AlignCenter, name_);

	// bounding rect (debug)
//	painter->setPen(Qt::red);
//	painter->drawRect(boundingRect());
}

void MemoryBlockItem::addMemEvent(const MemEvent& e)
{
	if (e.type == MemEvent::STORE)
	{
		for(int i = 0; i < e.size; i++)
		{
			MemCellItem* cell = getCell(e.addr + i);
			// TODO set value on cell
		}
	}
}

void MemoryBlockItem::moveAddr(quint64 addr)
{
	if (addr == addr_) return;
	prepareGeometryChange();
	auto newSize = size_ + (addr_ - addr);
	qDebug() << "mem block, moved, new addr=" << addr << ", old addr=" << addr_ << "new size=" << newSize << ", oldSize=" << size_;
	if (addr < addr_) // grow
	{
		//qDebug() << "mem block " << name_ << "grew from " << size_ << "to" << newSize;
		addr_ = addr;
		size_ = newSize;
	}
	else if (addr > addr_) // shrink
	{
		//qDebug() << "mem block " << name_ << "shrunk from " << size_ << "to" << newSize;
		//qDebug() << " new addr" << addr_;
		// delete cells before the new beginning
		auto end = cells_.lowerBound(addr);
		for (auto it = cells_.begin(); it != end;)
		{
			//qDebug() << "deleting cell " << it.key();
			delete it.value();
			it = cells_.erase(it);
		}
		addr_ = addr;
		size_ = newSize;
	}

	// reposition cells
	foreach(MemCellItem* cell, cells_)
	{
		QPointF newPos = cellPosition(cell->getAddr());
		cell->setPos(newPos);
	}

	update();
}

MemCellItem* MemoryBlockItem::getCell(quint64 addr)
{
	auto it = cells_.find(addr);
	if (it == cells_.end())
	{
		MemCellItem* cell = new MemCellItem(addr, this);

		cell->setPos(cellPosition(addr));
		cells_.insert(addr, cell);
		return cell;
	}
	return it.value();
}

void MemoryBlockItem::setColor(const QColor& color)
{
	color_ = color;
	update();
}

QPointF MemoryBlockItem::cellPosition(quint64 addr) const
{
	quint64 offset = addr - addr_;
	int row = offset / 8;
	int col = offset % 8;
	return QPointF(MARGIN + CELL_SIZE* col, MARGIN + HEADER_HEIGHT + CELL_SIZE * row);
}

void MemoryBlockItem::setName(const QString& name)
{
	name_ = name;
	update();
}

} // namespace Whiteboard
