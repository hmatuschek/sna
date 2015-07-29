#include <QApplication>

#include "sna.hh"
#include "logger.hh"
#include "mainwindow.hh"
#include <iostream>


int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  Logger::get().addHandler(new StreamLogHandler(LOG_DEBUG, std::cerr));

  SNA sna("/dev/tty.");

  MainWindow win(sna);
  win.show();

  app.exec();

  return 0;
}
