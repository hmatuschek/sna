#ifndef PORTDIALOG_HH
#define PORTDIALOG_HH

#include <QDialog>
#include <QComboBox>

class PortDialog : public QDialog
{
  Q_OBJECT

public:
  explicit PortDialog(QWidget *parent = 0);

  QString name();
  QString systemLocation();
  double refVoltage();

protected:
  QComboBox *_ports;
  QLineEdit *_ref;
};

#endif // PORTDIALOG_HH
