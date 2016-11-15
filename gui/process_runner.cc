#include "process_runner.hh"

#include <QJsonObject>
#include <QJsonDocument>
#include <QProcess>

#include <QDebug>

namespace Whiteboard {

ProcessRunner::ProcessRunner(QObject *p) :
	QObject(p)
{
}

void ProcessRunner::startExecutable(const QString& path)
{
	delete process_;
	process_ = new QProcess(this);

	process_->setProcessChannelMode(QProcess::ForwardedErrorChannel);
	process_->setReadChannel(QProcess::StandardOutput);

	connect(process_,  static_cast<void (QProcess::*)(QProcess::ProcessError)>(&QProcess::error), [](QProcess::ProcessError error) { qWarning("error"); });
	connect(process_,  static_cast<void (QProcess::*)(int)>(&QProcess::finished), []() { qWarning("finished"); });

	qDebug() << "sarting " << path;

	process_->start("/home/maciek/workspace/valgrind/inst/bin/valgrind", QStringList{"--tool=whiteboard", path});
}

QJsonObject ProcessRunner::getNextRecord()
{
	Q_ASSERT(process_);

	process_->waitForReadyRead();
	QByteArray line = process_->readLine();
	//qDebug() << "read line:" << line;
	QJsonParseError error;
	QJsonDocument doc = QJsonDocument::fromJson(line, &error);

	if (error.error != QJsonParseError::NoError)
		throw std::runtime_error("Error parsing json: " + error.errorString().toStdString());

	return doc.object();

}

} // namespace Whiteboard
