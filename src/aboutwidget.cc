#include "aboutwidget.hh"
#include <QFile>
#include <QIODevice>

AboutWidget::AboutWidget(QWidget *parent)
  : QTextEdit(parent)
{
  setReadOnly(true);
  QFile about(":/about.html");
  if (about.open(QIODevice::ReadOnly)) {
    setHtml(about.readAll());
  }
}


