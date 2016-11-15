#include "source_view.hh"
#include "ui_source_view.h"

#include <QDebug>
#include <QTextDocument>
#include <QFile>
#include <QTextBlock>

#include <stdexcept>

namespace Whiteboard {

SourceView::SourceView(QWidget* p) :
	QWidget(p),
	ui_(new Ui::SourceView)
{
	ui_->setupUi(this);
}

SourceView::~SourceView()
{
	delete ui_;
}

void SourceView::showSource(const QString& filePath, int line)
{
	qDebug() << "Showing source: " << filePath << line;

	QFile file(filePath);
	if (!file.open(QFile::ReadOnly | QFile::Text))
	{
		throw std::runtime_error("Error opening file");
	}

	QString text = QString::fromLocal8Bit(file.readAll());

	QTextDocument* doc = new QTextDocument(ui_->textBrowser_);
	doc->setPlainText(text);

	// highlight current line
	QTextCursor cursor(doc->findBlockByLineNumber(line));
	cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
	QTextBlockFormat format;
	format.setBackground(Qt::yellow);
	cursor.mergeBlockFormat(format);

	ui_->textBrowser_->setDocument(doc);
}

} // namespace Whiteboard
