#ifndef __SNA_SNA_HH__
#define __SNA_SNA_HH__


#include <QObject>
#include <QString>
#include <QSerialPort>
#include <QSettings>
#include <QByteArray>
#include <QTimer>
#include <inttypes.h>

/** Hardware interface class.
 * This class implements the communication with the SNA hardware. */
class SNA: public QObject
{
  Q_OBJECT

public:
  typedef enum {
    IDLE,
    GET_VALUE,
    SET_FREQUENCY,
    SHUTDOWN
  } Mode;

public:
  explicit SNA(const QString &portname, double vRef=2.5, double Fosc=125e6, double ppm=0, QObject *parent=0);

  double Fosc() const;

  bool sendGetValue();
  bool sendSetFrequency(double f);
  bool sendShutdown();

  QSettings &settings();

signals:
  void valueReceived(double sendGetValue);
  void frequencySet();
  void deviceShutdown();
  void error();

protected slots:
  void _onBytesWritten(qint64 n);
  void _onReadyRead();
  void _onTimeout();

protected:
  bool _send(const uint8_t *tx, size_t txlen, uint8_t *rx, size_t rxlen);
  bool _send(const uint8_t *tx, size_t txlen);

protected:
  QSettings _settings;
  QSerialPort _port;
  double _Fosc;
  double _ppm;
  double _ref;

  QByteArray _buffer;
  Mode    _mode;
  QTimer  _timeout;
};


#endif // __SNA_SNA_HH__
