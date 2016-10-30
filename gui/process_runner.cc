#include "process_runner.hh"

#include <QJsonObject>
#include <QProcess>

namespace Whiteboard {

ProcessRunner::ProcessRunner(QObject *p) :
	QObject(p)
{
}

void ProcessRunner::startExecutable(const QString& path)
{

}

} // namespace Whiteboard
