#include "stack_block_item.hh"

#include "mem_cell_item.hh"

#include <QPainter>
#include <QLinearGradient>
#include <QCursor>

namespace Whiteboard {

static const int MARGIN = 3;
static const int HEADER_HEIGHT = 20;

static const quint64 SIZE_MARGIN = 128;
static const quint64 FADEOUT_ROWS = 2;

static const QColor BACKGROUND = Qt::gray;

StackBlockItem::StackBlockItem(quint64 topAddr, QGraphicsItem* p)
	: QGraphicsItem(p), top_(topAddr)
{
	size_ = SIZE_MARGIN;
	setCursor(Qt::ArrowCursor);
}

void StackBlockItem::setStackBottom(quint64 addr)
{
	prepareGeometryChange();
	if (addr > top_)
	{
		size_ += (addr-top_);
		top_ = addr;
		update();
	}
	else if (addr < top_)
	{
		auto newSize = top_-addr + SIZE_MARGIN;

		if (newSize < size_)
		{
			qDebug() << "stack shrunk from " << size_ << "to" << newSize;
			qDebug() << " new bottom" << top_ - size_;
			// delete cells below bottom
			auto end = cells_.upperBound(top_ - size_);
			for (auto it = cells_.begin(); it != end;)
			{
				qDebug() << "deletimg cell " << it.key();
				delete it.value();
				it = cells_.erase(it);
			}
		}
		else qDebug() << "stack grew from " << size_ << "to" << newSize;

		size_ = newSize;

		update();
	}
}

QRectF StackBlockItem::boundingRect() const
{
	double h = (std::ceil(size_ / 8.0) + FADEOUT_ROWS) * CELL_SIZE + MARGIN*2+HEADER_HEIGHT;
	double w = MARGIN*2 + CELL_SIZE*8;
	return QRectF(0, 0, w, h);
}

void StackBlockItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	// background
	double h = std::ceil(size_ / 8.0) * CELL_SIZE + MARGIN*2 + HEADER_HEIGHT;
	double w = MARGIN*2 + CELL_SIZE*8;

	painter->fillRect(QRectF(0, 0, w, h), BACKGROUND);

	// fadeout
	QLinearGradient fadeout(QPointF(0, h), QPointF(0, h+FADEOUT_ROWS*CELL_SIZE));
	fadeout.setColorAt(0, BACKGROUND);
	fadeout.setColorAt(1, Qt::transparent);
	painter->fillRect(QRectF(0, h, w, FADEOUT_ROWS*CELL_SIZE), fadeout);

	// caption
	painter->drawText(QRectF(MARGIN, MARGIN, w-MARGIN, HEADER_HEIGHT), Qt::AlignCenter, "Stack");

	// bounding rect
	painter->setPen(Qt::red);
	painter->drawRect(boundingRect());
}

void StackBlockItem::addMemEvent(const MemEvent& e)
{
	for(int i = 0; i < e.size; i++)
	{
		MemCellItem* cell = getCell(e.addr + i);
	}
}

MemCellItem* StackBlockItem::getCell(quint64 addr)
{
	auto it = cells_.find(addr);
	if (it == cells_.end())
	{
		MemCellItem* cell = new MemCellItem(addr, this);

		quint64 offset = top_-addr-1;
		int row = offset / 8;
		int col = offset % 8;

		cell->setPos(MARGIN + CELL_SIZE* col, MARGIN + HEADER_HEIGHT + CELL_SIZE * row);
		cells_.insert(addr, cell);
		return cell;
	}
	return it.value();
}

} // namespace Whiteboard
