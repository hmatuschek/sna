#include "portdialog.hh"
#include <QSerialPortInfo>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialog>
#include <QLabel>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QSettings>


PortDialog::PortDialog(QWidget *parent) :
  QDialog(parent)
{
  setWindowTitle(tr("Select the serial interface ..."));
  _ports = new QComboBox();

  foreach (QSerialPortInfo info, QSerialPortInfo::availablePorts()) {
    _ports->addItem(info.portName(), info.systemLocation());
  }

  QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

  QSettings settings;
  _ref = new QLineEdit(QString::number(settings.value("ref", 2.5).toDouble()));
  _ref->setValidator(new QDoubleValidator(0, 2.5, 2));

  QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *layout = new QVBoxLayout();
  QFormLayout *form = new QFormLayout();
  form->addRow(tr("Interface"), _ports);
  form->addRow(tr("Reference voltage"), _ref);
  layout->addLayout(form);
  layout->addWidget(buttonBox);
  setLayout(layout);
}

QString
PortDialog::systemLocation() {
  return _ports->currentData().toString();
}

QString
PortDialog::name() {
  return _ports->currentText();
}

double
PortDialog::refVoltage() {
  return _ref->text().toDouble();
}
