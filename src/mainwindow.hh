#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QWidget>
#include <QMainWindow>
#include <QComboBox>
#include <QStackedWidget>

#include "sna.hh"
#include "livepanel.hh"
#include "scanpanel.hh"

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
};

#endif // MAINWINDOW_HH
