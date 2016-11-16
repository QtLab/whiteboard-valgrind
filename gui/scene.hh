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

private:

	MemoryBlockItem* stack_ = nullptr;

	QMap<quint64, MemoryBlockItem*> heap_;

};

} // namespace Whiteboard

