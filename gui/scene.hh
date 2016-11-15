#pragma once

#include <QGraphicsScene>

namespace Whiteboard {

class Scene : public QGraphicsScene
{
	Q_OBJECT
public:
	Scene(QObject* p = nullptr);

public slots:

signals:

private:

};

} // namespace Whiteboard

