#pragma once

#include <QObject>

class QJsonObject;
class QProcess;

namespace Whiteboard {

class ProcessRunner : public QObject
{
	Q_OBJECT
public:
	explicit ProcessRunner(QObject* p = nullptr);

	void startExecutable(const QString& path);

	QJsonObject getNextRecord();

private:

	QProcess* process_ = nullptr;
};

} // namespace Whiteboard

