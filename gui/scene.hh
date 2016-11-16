#pragma once

#include "mem_event.hh"

#include <QGraphicsScene>

namespace Whiteboard {

class MemoryBlockItem;

class Scene : public QGraphicsScene
{
	Q_OBJECT
public:
	Scene(QObject* p = nullptr);

	void setViewportSize(const QSize& sz);

public slots:

	void onStackChange(quint64 addr);
	void onMemEvent(const MemEvent& e);
	void onHeapEvent(const HeapEvent& e);

private:

	// finds a place for a block of specific size
	QPointF findPlaceForBlock(const QSizeF& sz) const;

	MemoryBlockItem* stack_ = nullptr;

	QMap<quint64, MemoryBlockItem*> heap_;
	QSize viewportSize_;

};

} // namespace Whiteboard

