#ifndef __SNA_SNA_HH__
#define __SNA_SNA_HH__


#include <QObject>
#include <QString>
#include <QSerialPort>
#include <QSettings>


class SNA: public QObject
{
  Q_OBJECT

public:
  explicit SNA(const QString &portname, double Fosc=125e6, double ppm=0, QObject *parent=0);

  double Fosc() const;

  double value();
  bool setFrequency(double f);
  double valueAt(double f);

  QSettings &settings();

protected:
  bool _send(const uint8_t *tx, size_t txlen, uint8_t *rx, size_t rxlen);

protected:
  QSettings _settings;
  QSerialPort _port;
  double _Fosc;
  double _ppm;
};


#endif // __SNA_SNA_HH__
