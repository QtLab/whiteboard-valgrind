#include "mem_cell_item.hh"

#include <QPainter>

namespace Whiteboard {

MemCellItem::MemCellItem(quint64 addr, QGraphicsItem* p)
	: QGraphicsItem(p), addr_(addr)
{
	setToolTip(QString::number(addr, 16));
}

QRectF MemCellItem::boundingRect() const
{
	return QRectF(0, 0, CELL_SIZE, CELL_SIZE);
}

void MemCellItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	QRectF r(0, 0, CELL_SIZE - 1, CELL_SIZE - 1);
	painter->fillRect(r, Qt::white);
}

} // namespace Whiteboard
