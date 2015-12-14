// TerraMA2
#include "LoggerFormWidget.hpp"
#include "ui_LoggerFormWidget.h"

// QT
#include <QDebug>
#include <QListWidgetItem>
#include <QFile>
#include <QFileSystemWatcher>

struct LoggerFormWidget::Impl
{
  Impl()
    : ui_(new Ui::LoggerFormWidget), watcher_(), file_()
  {

  }

  ~Impl()
  {
    delete ui_;
  }

  Ui::LoggerFormWidget* ui_;
  QFileSystemWatcher watcher_;
  QFile file_;
};

LoggerFormWidget::LoggerFormWidget(QWidget *parent) :
  QWidget(parent), pimpl_(new Impl)
{
  pimpl_->ui_->setupUi(this);
}

LoggerFormWidget::~LoggerFormWidget()
{
  delete pimpl_;
}

void LoggerFormWidget::setPath(const QString& path)
{
  pimpl_->file_.setFileName(path);
}

void LoggerFormWidget::refresh()
{
  if (!pimpl_->file_.isOpen())
  {
    pimpl_->file_.open(QFile::ReadOnly);
  }

  QString text = pimpl_->file_.readAll();
  if (!text.isEmpty())
    qDebug() << text;
//    pimpl_->ui_->loggerPlain->appendPlainText(text);

//  pimpl_->ui_->loggerPlain->verticalScrollBar()->setValue(pimpl_->ui_->loggerPlain->verticalScrollBar()->maximum());
}
