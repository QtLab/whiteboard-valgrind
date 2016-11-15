#pragma once

#include "mem_event.hh"

#include <QGraphicsScene>

namespace Whiteboard {

class Scene : public QGraphicsScene
{
	Q_OBJECT
public:
	Scene(QObject* p = nullptr);

public slots:

	void onStackChange(quint64 addr);
	void onMemEvent(const MemEvent& e);

private:

};

} // namespace Whiteboard

