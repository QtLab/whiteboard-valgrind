#pragma once

#include "mem_event.hh"

#include <QGraphicsItem>

namespace Whiteboard {

class MemCellItem;

class MemoryBlockItem: public QGraphicsItem
{
public:
	MemoryBlockItem(quint64 addr, quint64 size, QGraphicsItem* p = nullptr);

	// QGraphicsItem
	QRectF boundingRect() const override;
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

	// Change the begginnig address, w/o changing the end address.
	// Used to move the top of the stack
	void moveAddr(quint64 addr);

	bool ownsAddress(quint64 addr) const { return addr >= addr_ && addr <= (addr_ + size_); }
	void addMemEvent(const MemEvent& e);

	QString getName() const { return name_; }
	void setName(const QString& name);

	QColor getColor() const { return color_; }
	void setColor(const QColor& color);

private:

	QPointF cellPosition(quint64 addr) const;
	MemCellItem* getCell(quint64 addr);

	QMap<quint64, MemCellItem*> cells_;

	quint64 addr_;
	quint64 size_;

	QString name_;
	QColor color_ = Qt::magenta;
};

} // namespace Whiteboard

