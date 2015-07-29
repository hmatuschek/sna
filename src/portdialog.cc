#include "portdialog.hh"
#include <QSerialPortInfo>
#include <QVBoxLayout>
#include <QDialog>
#include <QLabel>
#include <QDialogButtonBox>


PortDialog::PortDialog(QWidget *parent) :
  QDialog(parent)
{
  _ports = new QComboBox();

  foreach (QSerialPortInfo info, QSerialPortInfo::availablePorts()) {
    _ports->addItem(info.portName(), info.systemLocation());
  }

  QLabel *label = new QLabel(tr("Select interface:"));

  QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

  QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(label);
  layout->addWidget(_ports);
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
