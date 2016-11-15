#pragma once

#include <QGraphicsItem>

namespace Whiteboard {

static const int CELL_SIZE = 20;

class MemCellItem : public QGraphicsItem
{
public:
	MemCellItem(quint64 addr, QGraphicsItem* p = nullptr);

	QRectF boundingRect() const override;
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

	quint64 getAddr() const { return addr_; }

private:

	quint64 addr_;
};

} // namespace Whiteboard

