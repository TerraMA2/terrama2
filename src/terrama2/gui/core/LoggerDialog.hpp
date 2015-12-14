#ifndef __TERRAMA2_GUI_CORE_LOGGERDIALOG_HPP__
#define __TERRAMA2_GUI_CORE_LOGGERDIALOG_HPP__

#include <QWidget>

class LoggerDialog : public QWidget
{
  Q_OBJECT

  public:
    LoggerDialog(QWidget* parent = 0);
    ~LoggerDialog();

  private:
    //! It refreshes the text plain with log content
    void refresh();

  private slots:
    //! Slot triggered when clear button has been clicked and it clears text plain
    void onClearBtnClicked();

    //! Slot triggered when file log has been changed externally.
    void onLogChanged(const QString& path);

  private:
    struct Impl; //!< Pimpl idiom

    Impl* pimpl_;
};

#endif // __TERRAMA2_GUI_CORE_LOGGERDIALOG_HPP__
