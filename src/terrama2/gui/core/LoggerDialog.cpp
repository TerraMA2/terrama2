// TerraMA2
#include "LoggerDialog.hpp"
#include "ui_LoggerDialogForm.h"

#include "../../core/Logger.hpp"

// QT
#include <QFile>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QScrollBar>
#include <QDebug>
#include <QSharedPointer>
#include <QMessageBox>

struct LoggerDialog::Impl
{
  Impl()
    : ui_(new Ui::LoggerDialogForm), watcher_(), file_()
  {

  }

  ~Impl()
  {
    delete ui_;
  }

  Ui::LoggerDialogForm* ui_;
  QFileSystemWatcher watcher_;
  QFile file_;
};

LoggerDialog::LoggerDialog(QWidget *parent) :
  QDialog(parent), pimpl_(new Impl)
{
  pimpl_->ui_->setupUi(this);

  // TODO: get it from logger
  QString path("/home/raphael/Documents/tmp/terrama2.log");

  pimpl_->file_.setFileName(path);
  pimpl_->file_.open(QFile::ReadOnly);
  if (pimpl_->file_.isOpen())
    pimpl_->ui_->loggerPlain->setPlainText(pimpl_->file_.readAll());
  pimpl_->watcher_.addPath(path);

  connect(pimpl_->ui_->clearButton, SIGNAL(clicked()), SLOT(onClearBtnClicked()));
  connect(&pimpl_->watcher_, SIGNAL(fileChanged(const QString&)), SLOT(onLogChanged(const QString&)));

  refresh();
}

LoggerDialog::~LoggerDialog()
{
  delete pimpl_;
}

void LoggerDialog::refresh()
{
  if (!pimpl_->file_.isOpen())
  {
    pimpl_->file_.open(QFile::ReadOnly);
  }

  QString text = pimpl_->file_.readAll();
  if (!text.isEmpty())
    pimpl_->ui_->loggerPlain->appendPlainText(text);

  pimpl_->ui_->loggerPlain->verticalScrollBar()->setValue(pimpl_->ui_->loggerPlain->verticalScrollBar()->maximum());
}

void LoggerDialog::onClearBtnClicked()
{
  pimpl_->ui_->loggerPlain->clear();
}

void LoggerDialog::onLogChanged(const QString& path)
{
  // temp
  QFileInfo fileInfo(path);

  if (fileInfo.exists())
  {
    if (path.toStdString() == terrama2::core::Logger::getInstance().path())
      refresh();
  }
  else
  {
    const QString message = tr("The terrama2 log does not exist or has been moved.");
    QMessageBox::warning(this, tr("TerraMA2 Log Error"), message);
    TERRAMA2_LOG_ERROR() << message.toStdString();
  }
}
