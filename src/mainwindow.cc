#include "mainwindow.hh"
#include <QVBoxLayout>
#include "aboutwidget.hh"


MainWindow::MainWindow(SNA &sna, QWidget *parent) :
  QMainWindow(parent), _sna(sna)
{
  setWindowTitle(tr("Scalar Network Analyzer"));

  _taskSelect = new QComboBox();
  _taskSelect->addItem(tr("Scan"));
  _taskSelect->addItem(tr("Poll"));
  _taskSelect->addItem(tr("About SNA"));

  _view = new QStackedWidget();
  _scan = new ScanPanel(_sna);
  _poll = new PollPanel(_sna);

  _view->addWidget(_scan);
  _view->addWidget(_poll);
  _view->addWidget(new AboutWidget());

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(_taskSelect, 0);
  layout->addWidget(_view, 1);
  layout->setContentsMargins(0,0,0,0);
  layout->setSpacing(5);

  QWidget *panel = new QWidget();
  panel->setLayout(layout);
  setCentralWidget(panel);

  QObject::connect(_taskSelect, SIGNAL(currentIndexChanged(int)), _view, SLOT(setCurrentIndex(int)));
  QObject::connect(_scan, SIGNAL(started()), this, SLOT(onTaskStarted()));
  QObject::connect(_scan, SIGNAL(stopped()), this, SLOT(onTaskStopped()));
  QObject::connect(_poll, SIGNAL(started()), this, SLOT(onTaskStarted()));
  QObject::connect(_poll, SIGNAL(stopped()), this, SLOT(onTaskStopped()));
}


void
MainWindow::onTaskStarted() {
  _taskSelect->setEnabled(false);
}

void
MainWindow::onTaskStopped() {
  _taskSelect->setEnabled(true);
}

