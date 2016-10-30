#pragma once

#include <QJsonObject>
#include <QObject>

class QJsonObject

namespace Whiteboard {

class ProcessRunner : public QObject
{
	Q_OBJECT
public:
	explicit ProcessRunner(QObject* p = nullptr);

	void startExecutable(const QString& path);

signals:

	void newRecord(const QJsonObject& obj);

public slots:

};

} // namespace Whiteboard

