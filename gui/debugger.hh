#pragma once

#include "mem_event.hh"

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

	// memory events
	void stackChange(quint64 addr);
	void memEvent(const MemEvent& ev);
	void heapEvent(const HeapEvent& ev);

	void canRun(bool);

public slots:

	void openExecutable(const QString& path);
	void stepInto();

private:

	void processUntilNextLine();
	void processNextRecord(const QJsonObject& obj);

	void onLineStep(const QJsonObject& obj);
	void onMemStore(const QJsonObject& obj);
	void onMemLoad(const QJsonObject& obj);
	void onAllocation(const QJsonObject& obj);
	void onFree(const QJsonObject& obj);

	ProcessRunner* runner_ = nullptr;
	bool running_ = false;
};

} // namespace Whiteboard

