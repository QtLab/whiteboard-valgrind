#pragma once

#include "mem_event.hh"

#include <QGraphicsScene>

namespace Whiteboard {

class StackBlockItem;

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

	StackBlockItem* stack_ = nullptr;

};

} // namespace Whiteboard

