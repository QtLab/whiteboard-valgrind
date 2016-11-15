#include "stack_block_item.hh"

#include <QPainter>
#include <QLinearGradient>

namespace Whiteboard {

static const int CELL_SIZE = 20;
static const int MARGIN = 3;
static const int HEADER_HEIGHT = 20;

static const quint64 SIZE_MARGIN = 128;
static const quint64 FADEOUT_ROWS = 2;

static const QColor BACKGROUND = Qt::gray;

StackBlockItem::StackBlockItem(quint64 topAddr, QGraphicsItem* p)
	: QGraphicsItem(p), top_(topAddr)
{
	size_ = SIZE_MARGIN;
}

void StackBlockItem::setStackTop(quint64 addr)
{
	// TODO
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
}

void StackBlockItem::addMemEvent(const MemEvent& e)
{
	// TODO
	qDebug() << "stack mem event";
}

} // namespace Whiteboard
