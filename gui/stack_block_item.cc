#include "stack_block_item.hh"

#include "mem_cell_item.hh"

#include <QPainter>
#include <QLinearGradient>
#include <QCursor>

namespace Whiteboard {

StackBlockItem::StackBlockItem(quint64 addr, quint64 size, QGraphicsItem* p)
	: MemoryBlockItem(addr, size, p)
{
}



} // namespace Whiteboard
