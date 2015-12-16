#include "LoggerWidget.hpp"
#include "ui_LoggerWidget.h"

#include <QPushButton>

struct LoggerWidget::Impl
{
  Impl()
      : ui_(new Ui::LoggerWidget)
  {
  }

  ~Impl()
  {
    delete ui_;
  }

  Ui::LoggerWidget* ui_;
};

LoggerWidget::LoggerWidget(QWidget *parent) :
    QWidget(parent), pimpl_(new Impl)
{
  pimpl_->ui_->setupUi(this);
}

LoggerWidget::~LoggerWidget()
{
  delete pimpl_;
}

const QPushButton *LoggerWidget::button() const
{
  return pimpl_->ui_->logger;
}
