#pragma once

#include <QGraphicsScene>

namespace Whiteboard {

class Scene : public QGraphicsScene
{
	Q_OBJECT
public:
	Scene(QObject* p = nullptr);

public slots:

	void openExecutable(const QString& path);
};

} // namespace Whiteboard

