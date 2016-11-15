#include "debugger.hh"

#include "process_runner.hh"

#include <QJsonObject>

#include <QJsonDocument> //TODO debug
#include <QDebug>
#include <QDir>
#include <QCoreApplication>

namespace Whiteboard {

Debugger::Debugger(QObject* p) : QObject(p)
{
	runner_ = new ProcessRunner(this);
}

void Debugger::openExecutable(const QString& path)
{
	emit statusChanged(QString(tr("Starting program: %1...")).arg(path));
	runner_->startExecutable(path);
	QCoreApplication::processEvents();
	processUntilNextLine();
	emit statusChanged(tr("Paused"));
	running_ = true;
}

void Debugger::stepInto()
{
	if (running_)
	{
		processUntilNextLine();
	}
}

void Debugger::processUntilNextLine()
{
	while(true)
	{
		QJsonObject obj = runner_->getNextRecord();
		if (obj.isEmpty())
		{
			emit statusChanged("Process finished");
			running_ = false;
			break;
		}
		processNextRecord(obj);
		QString action = obj["action"].toString();
		Q_ASSERT(!action.isEmpty());
		if (action == "line-step")
			break;
	}
}

void Debugger::processNextRecord(const QJsonObject& obj)
{
	QString action = obj["action"].toString();

	if (action == "line-step")
		onLineStep(obj);
	else if (action == "stack-change")
		emit stackChange(obj["addr"].toString().toLongLong());
	else if (action == "mem-store")
		onMemStore(obj);
	else if (action == "mem-load")
		onMemLoad(obj);

	else
	// TODO
		qDebug() << QJsonDocument(obj).toJson();
}

void Debugger::onLineStep(const QJsonObject& obj)
{
	QDir dir = obj["dir"].toString();
	QString file = obj["file"].toString();
	int line = obj["line"].toInt();

	QFileInfo f(dir, file);

	emit sourceLineReached(f.absoluteFilePath(), line);
}

void Debugger::onMemStore(const QJsonObject& obj)
{
	MemEvent event;
	event.addr = obj["addr"].toString().toLongLong();
	event.size = obj["size"].toInt();
	if (event.size == 8)
		event.data = obj["data"].toString().toLongLong();
	event.type = MemEvent::STORE;

	emit memEvent(event);
}

void Debugger::onMemLoad(const QJsonObject& obj)
{
	MemEvent event;
	event.addr = obj["addr"].toString().toLongLong();
	event.size = obj["size"].toInt();
	event.type = MemEvent::LOAD;

	emit memEvent(event);
}

} // namespace Whiteboard
