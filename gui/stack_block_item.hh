#pragma once

#include "mem_event.hh"

#include <QGraphicsItem>

namespace Whiteboard {

class StackBlockItem : public QGraphicsItem
{
public:
	StackBlockItem(quint64 topAddr, QGraphicsItem* p = nullptr);

	void setStackTop(quint64 addr);

	QRectF boundingRect() const override;
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

	bool ownsAddress(quint64 addr) const { return addr <= top_ && addr >= (top_-size_); }
	void addMemEvent(const MemEvent& e);

private:

	quint64 top_;
	quint64 size_;

};

} // namespace Whiteboard

