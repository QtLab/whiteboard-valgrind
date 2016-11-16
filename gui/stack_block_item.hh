#pragma once

#include "memory_block_item.hh"

#include <QGraphicsItem>
#include <QMap>

namespace Whiteboard {


class StackBlockItem : public MemoryBlockItem
{
public:
	StackBlockItem(quint64 addr, quint64 size, QGraphicsItem* p = nullptr);

};

} // namespace Whiteboard

