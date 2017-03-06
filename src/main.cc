#include <QApplication>

#include "sna.hh"
#include "logger.hh"
#include "mainwindow.hh"
#include "portdialog.hh"
#include <iostream>


int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  app.setApplicationName("SNA");
  app.setOrganizationName("io.github.hmatuschek");
  app.setOrganizationDomain("hmatuschek.github.io");

  Logger::get().addHandler(new StreamLogHandler(LOG_DEBUG, std::cerr));

  PortDialog dialog;
  if (QDialog::Accepted != dialog.exec()) { return 0; }

  QSettings settings; settings.setValue("ref", dialog.refVoltage());

  QString systemLocation = dialog.systemLocation();
  SNA sna(systemLocation, dialog.refVoltage());

  MainWindow win(sna);
  win.show();

  app.exec();

  return 0;
}
