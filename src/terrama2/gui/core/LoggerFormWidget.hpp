#ifndef __TERRAMA2_GUI_CORE_LOGGERFORMWIDGET_HPP__
#define __TERRAMA2_GUI_CORE_LOGGERFORMWIDGET_HPP__

#include <QWidget>


class LoggerFormWidget : public QWidget
{
  Q_OBJECT

  public:
    explicit LoggerFormWidget(QWidget *parent = 0);
    ~LoggerFormWidget();

    void setPath(const QString&);
    void refresh();

  private:
    struct Impl;

    Impl* pimpl_;
};

#endif // __TERRAMA2_GUI_CORE_LOGGERFORMWIDGET_HPP__
