#include "process_runner.hh"

#include <QJsonObject>
#include <QJsonDocument>
#include <QProcess>
#include <QDir>
#include <QLocalSocket>

#include <QDebug>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

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

	// create fifo
	fifoPath_ = QDir::tempPath() + QString("/whiteborad-%1.fifo").arg(::getpid());
	qDebug() << "creating fifo at:" << fifoPath_;
	::unlink(fifoPath_.toLocal8Bit().data());
	int res = ::mkfifo(fifoPath_.toLocal8Bit().data(), S_IRWXU);
	if (res != 0)
	{
		::perror("Unable to create fifo");
		qFatal("Unable to create fifo");
	}


	qDebug() << "sarting " << path;

	process_->start("/home/maciek/workspace/valgrind/inst/bin/valgrind",
			QStringList{"--tool=whiteboard", QString("--output=%1").arg(fifoPath_), path});

	int fd = ::open(fifoPath_.toLocal8Bit().data(), O_RDONLY);
	input_ = new QLocalSocket(this);
	if (!input_->setSocketDescriptor(fd, QLocalSocket::ConnectedState, QLocalSocket::ReadOnly))
	{
		qFatal("Failed to open fifo");
	}


}

QJsonObject ProcessRunner::getNextRecord()
{
	Q_ASSERT(process_);

	input_->waitForReadyRead();
	QByteArray line = input_->readLine();
	//qDebug() << "read line:" << line;
	if (line.isEmpty())
	{
		//cleanup
		input_->close();
		delete input_;
		::unlink(fifoPath_.toLocal8Bit().data());

		return {};
	}
	QJsonParseError error;
	QJsonDocument doc = QJsonDocument::fromJson(line, &error);

	if (error.error != QJsonParseError::NoError)
		throw std::runtime_error("Error parsing json: " + error.errorString().toStdString());

	return doc.object();

}

} // namespace Whiteboard
