#pragma once

#include "mem_event.hh"

#include <QGraphicsItem>

namespace Whiteboard {

class MemoryBlockItem: public QGraphicsItem
{
public:
	MemoryBlockItem(quint64 addr, quint64 len, QGraphicsItem* p = nullptr);

	void addMemEvent(const MemEvent& e);

	QRectF boundingRect() const override;

private:

	quint64 addr_;
	quint64 length_;
};

} // namespace Whiteboard

