#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QWidget>
#include <QMainWindow>
#include <QComboBox>
#include <QStackedWidget>

#include "sna.hh"
#include "livepanel.hh"
#include "scanpanel.hh"
#include <QStatusBar>
#include "logger.hh"

class StatusLogHandler: public QObject, public LogHandler
{
  Q_OBJECT

public:
  StatusLogHandler(QStatusBar *status);

  void handle(const LogMessage &msg);

protected slots:
  void _onStatusDeleted();

protected:
  QStatusBar *_status;
};


class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(SNA &_sna, QWidget *parent = 0);

protected slots:
  void onTaskStarted();
  void onTaskStopped();

protected:
  SNA &_sna;
  QComboBox *_taskSelect;
  QStackedWidget *_view;

  PollPanel *_poll;
  ScanPanel *_scan;
  QStatusBar *_status;
};

#endif // MAINWINDOW_HH
