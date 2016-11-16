#pragma once

#include <QGraphicsItem>

namespace Whiteboard {

enum class ItemType
{
	MEMORY_BLOCK = QGraphicsItem::UserType+100,
	OTHER
};

}
