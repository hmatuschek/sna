#ifndef ABOUTWIDGET_HH
#define ABOUTWIDGET_HH

#include <QTextEdit>

/** Simple widget to display the name and license. */
class AboutWidget : public QTextEdit
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit AboutWidget(QWidget *parent = 0);
};

#endif // ABOUTWIDGET_HH
