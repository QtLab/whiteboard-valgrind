#pragma once

#include <QObject>

namespace Whiteboard {

class ProcessRunner;

class Debugger : public QObject
{
	Q_OBJECT
public:
	explicit Debugger(QObject* p = nullptr);

signals:

	void sourceLineReached(const QString& path, int line);
	void statusChanged(const QString& status);

public slots:

	void openExecutable(const QString& path);
	void stepInto();

private:

	void processUntilNextLine();
	void processNextRecord(const QJsonObject& obj);

	void onLineStep(const QJsonObject& obj);

	ProcessRunner* runner_ = nullptr;
};

} // namespace Whiteboard

