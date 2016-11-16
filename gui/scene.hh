#pragma once

#include "mem_event.hh"

#include <QGraphicsScene>
#include <QQueue>

#include <functional>

namespace Whiteboard {

class MemoryBlockItem;
class Animations;

class Scene : public QGraphicsScene
{
	Q_OBJECT
public:
	Scene(QObject* p = nullptr);

	void setViewportSize(const QSize& sz);

	// if is ready for next step
	bool isReady() const;

public slots:

	void onStackChange(quint64 addr);
	void onMemEvent(const MemEvent& e);
	void onHeapEvent(const HeapEvent& e);

signals:

	void ready();

private:

	template<typename EventT>
	void executeOrQueue(const EventT& e);

	void executeEvent(const MemEvent& e, qint64 now);
	void executeEvent(const HeapEvent& e, qint64 now);

	using Event = std::function<void(qint64 now)>;

	void processAnimations();

	// finds a place for a block of specific size
	QPointF findPlaceForBlock(const QSizeF& sz) const;

	MemoryBlockItem* stack_ = nullptr;

	QMap<quint64, MemoryBlockItem*> heap_;
	QSize viewportSize_;

	QTimer* timer_;
	QQueue<Event> awaitingEvents_;
	Animations* animations_;
};

} // namespace Whiteboard

