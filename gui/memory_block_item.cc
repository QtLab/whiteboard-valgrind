#include "memory_block_item.hh"

namespace Whiteboard {

static const int CELL_SIZE = 20;
static const int MARGIN = 3;
static const int HEADER_HEIGHT = 20;

MemoryBlockItem::MemoryBlockItem(quint64 addr, quint64 len, QGraphicsItem* p)
	: QGraphicsItem(p), addr_(addr), length_(len)
{
}

QRectF MemoryBlockItem::boundingRect() const
{
	int h = std::ceil(length_ / 8.0);
	return QRectF(0, 0, MARGIN*2 + CELL_SIZE*4, CELL_SIZE*h + MARGIN*2+HEADER_HEIGHT);
}

} // namespace Whiteboard
