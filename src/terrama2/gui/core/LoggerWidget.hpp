#ifndef __TERRAMA2_GUI_CORE_LOGGERDIALOG_HPP__
#define __TERRAMA2_GUI_CORE_LOGGERDIALOG_HPP__

#include <QWidget>

class QPushButton;

class LoggerWidget : public QWidget
{
  Q_OBJECT

  public:
    explicit LoggerWidget(QWidget* parent = 0);
    ~LoggerWidget();

    const QPushButton* button() const;

  private:
    struct Impl;

    Impl* pimpl_;

};

#endif // __TERRAMA2_GUI_CORE_LOGGERDIALOG_HPP__
