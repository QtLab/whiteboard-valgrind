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
		return onLineStep(obj);

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

} // namespace Whiteboard
