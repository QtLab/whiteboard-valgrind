#include "animations.hh"

#include "mem_cell_item.hh"\

#include <QPainter>
#include <QGradient>
#include <QDebug>

#include <cassert>

namespace Whiteboard {

class Animation : public QGraphicsItem
{
public:
	Animation(QGraphicsItem* p) : QGraphicsItem(p) { setZValue(1.0); }
	virtual void advance(qint64 now) = 0;
};

// Mem load.
class MemLoad : public Animation
{
public:
	MemLoad(MemCellItem* p, qint64 start, int size) : Animation(p), start_(start), size_(size)
	{
		a_ = 1.0;
	}

	void advance(qint64 now) override
	{
		auto age = now - start_;
		if (age >= 1000)
			a_ = 0.0;
		else
			a_ = 1.0 - age / 1000.0;
		update();
	}

	QRectF boundingRect() const override
	{
		return QRectF(0, 0, CELL_SIZE*size_, CELL_SIZE);
	}

	void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
	{
		QColor fill;
		fill.setRgbF(0.5, 0.5, 1.0, a_);
		painter->fillRect(boundingRect(), fill);
	}

private:

	double a_;
	qint64 start_;
	int size_;
};

// Mem store
class MemStore : public Animation
{
public:
	MemStore(MemCellItem* p, qint64 start, int size) : Animation(p), start_(start), size_(size)
	{
		a_ = 1.0;
	}

	void advance(qint64 now) override
	{
		auto age = now - start_;
		if (age >= 1000)
			a_ = 0.0;
		else
			a_ = 1.0 - age / 1000.0;
		update();
	}

	QRectF boundingRect() const override
	{
		return QRectF(0, 0, CELL_SIZE*size_, CELL_SIZE);
	}

	void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
	{
		QColor fill;
		fill.setRgbF(1.0, 0.5, 0.5, a_);
		painter->fillRect(boundingRect(), fill);
	}

private:

	double a_;
	qint64 start_;
	int size_;
};


Animations::Animations(QObject *p) : QObject(p)
{
}

void Animations::advance(qint64 now)
{
	foreach(Animation* a, animations_)
	{
		a->advance(now);
	}

	auto end = animations_.upperBound(now);
	for(auto it = animations_.begin(); it != end; it = animations_.erase(it))
	{
		delete *it;
	}
}

void Animations::addMemLoad(MemCellItem* cell, int size, qint64 now, bool continuation)
{
	assert(continuation || canAddNew(now));

	//qDebug() << "load " << size  << "@" << cell->getAddr();

	Animation* a = new MemLoad(cell, now, size);
	cell->setZValue(0.5);
	animations_.insert(now + 1000, a); // kill after 1 sec
	if (!continuation)
		blockedUntil_ = now + 100; // block subsequent animations for 100ms
}

void Animations::addMemStore(MemCellItem* cell, int size, qint64 now, bool continuation)
{
	assert(continuation || canAddNew(now));

	//qDebug() << "store " << size  << "@" << cell->getAddr();

	Animation* a = new MemStore(cell, now, size);
	cell->setZValue(0.5);
	animations_.insert(now + 1000, a); // kill after 1 sec
	if (!continuation)
		blockedUntil_ = now + 100; // block subsequent animations for 100ms
}

} // namespace Whiteboard
