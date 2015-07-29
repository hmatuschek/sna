#include "plot.hh"
#include <inttypes.h>
#include <QPainter>
#include <QPaintEvent>
#include <QFont>
#include <QString>
#include <QRectF>
#include <cmath>
#include "logger.hh"


Plot::Plot(QWidget *parent) :
  QWidget(parent), _Fmin(0), _Fmax(0), _dBmMin(0), _dBmMax(0), _F(), _dBm(), _pointer(false)
{
  setMinimumSize(640, 480);
}

void
Plot::plot(const std::vector<double> &F, const std::vector<double> &dBm, size_t N) {
  _F.resize(N); _dBm.resize(N);
  for (size_t i=0; i<N; i++) {
    _F[i] = F[i]; _dBm[i] = dBm[i];
    if (0 == i) {
      _Fmin   = _Fmax   = F[i];
      _dBmMin = _dBmMax = dBm[i];
    } else {
      _Fmin   = std::min(_Fmin, F[i]);
      _Fmax   = std::max(_Fmax, F[i]);
      _dBmMin = std::min(_dBmMin, dBm[i]);
      _dBmMax = std::max(_dBmMax, dBm[i]);
    }
  }
  _dBmMin = std::floor(_dBmMin-0.05*_dBmMin);
  _dBmMax = std::ceil(_dBmMax+0.05*_dBmMax);
  // Reset pointer
  _pointer = false;
  // Force redraw
  update();
}


void
Plot::paintEvent(QPaintEvent *evt) {
  // First, paint widget background
  QWidget::paintEvent(evt);

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setClipRect(evt->rect());
  painter.save();

  painter.fillRect(evt->rect(), Qt::white);

  _drawGraph(painter);
  _drawAxes(painter);
  _drawPointer(painter);

  painter.restore();
}


void
Plot::_drawAxes(QPainter &painter) {
  int32_t height = this->size().height();
  int32_t width = this->size().width();

  QPen pen(Qt::black);
  pen.setWidth(1); pen.setCosmetic(true);
  painter.setPen(pen);

  // Draw axes ticks:
  double dF = (_Fmax-_Fmin)/8;
  double F  = _Fmin;

  QFont font; font.setPointSize(10);
  QFontMetrics fm(font);
  painter.setFont(font);

  pen.setStyle(Qt::DashLine);
  painter.setPen(pen);
  for (size_t i=0; i<9; i++, F+=dF) {
    int x  = (i*width)/8;
    for (size_t j=0; j<9; j++) {
      int    y  = (j*height)/8;
      painter.drawPoint(x, y);
    }

    if ((0!=i)&&(8!=i)) {
      QString label;
      if (F < 1e3) { label = tr("%1 Hz").arg(QString::number(F, 'f', 2)); }
      else if (F < 1e6) { label = tr("%1 kHz").arg(QString::number(F/1e3, 'f', 2)); }
      else { label = tr("%1 MHz").arg(QString::number(F/1e6, 'f', 2)); }
      QRectF bb = fm.boundingRect(label);
      painter.drawText(x-bb.width()/2, 3+fm.ascent(), label);
      painter.drawText(x-bb.width()/2, height-3-fm.descent(), label);
    }
  }

  double dv = (_dBmMax-_dBmMin)/8, v  = _dBmMax;
  for (size_t i=1; i<8; i++, v-=dv) {
    int y = (i*height)/8;
    QString label = tr("%1 dBm").arg(v);
    QRectF bb = fm.boundingRect(label);
    float shift = bb.height()/2 - fm.ascent();
    painter.drawText(3, y-shift, label);
    painter.drawText(width-bb.width()-3, y-shift, label);
  }
}


void
Plot::_drawGraph(QPainter &painter) {
  painter.save();

  QPen pen(Qt::blue);
  pen.setWidth(2);
  pen.setStyle(Qt::SolidLine);
  painter.setPen(pen);

  size_t N = _F.size();
  double dy = (_dBmMax-_dBmMin)/height();
  // If there are less samples than pixels -> iterate over samples
  for (size_t j=1; j<N; j++) {
    int x1 = ((j-1)*width())/N;
    int y1 = height()-(_dBm[j-1]-_dBmMin)/dy;
    int x2 = (j*width())/N;
    int y2 = height()-(_dBm[j]-_dBmMin)/dy;
    painter.drawLine(x1,y1, x2,y2);
  }

  painter.restore();
}

void
Plot::_drawPointer(QPainter &painter) {
  if (! _pointer) { return; }

  painter.save();

  QPen pen(Qt::black);
  pen.setWidth(2);
  pen.setStyle(Qt::SolidLine);
  painter.setPen(pen);

  painter.drawLine(_pointer_pos.x()-5,_pointer_pos.y()-5,
                   _pointer_pos.x()+5,_pointer_pos.y()-5);
  painter.drawLine(_pointer_pos.x()+5,_pointer_pos.y()-5,
                   _pointer_pos.x(),_pointer_pos.y());
  painter.drawLine(_pointer_pos.x(), _pointer_pos.y(),
                   _pointer_pos.x()-5,_pointer_pos.y()-5);

  QString label;
  if (_pointer_val.x()<1e3) {
    label = tr("%1 dBm @%2 Hz").arg(_pointer_val.y()).arg(_pointer_val.x());
  } else if (_pointer_val.x()<1e6) {
    label = tr("%1 dBm @%2 kHz").arg(_pointer_val.y()).arg(_pointer_val.x()/1e3);
  } else {
    label = tr("%1 dBm @%2 MHz").arg(_pointer_val.y()).arg(_pointer_val.x()/1e6);
  }
  painter.drawText(_pointer_pos.x()+10, _pointer_pos.y()-10, label);
  painter.restore();
}

void
Plot::mouseReleaseEvent(QMouseEvent *evt) {
  QWidget::mouseReleaseEvent(evt);
  QPoint p = evt->pos();
  double dx = (_Fmax-_Fmin)/width();
  double dy = (_dBmMax-_dBmMin)/height();
  // Search for index
  size_t idx=0;
  for(; idx<_F.size(); idx++) {
    if (_F[idx]>(_Fmin+p.x()*dx)) { break; }
  }
  // Update pointer position
  if (idx < _F.size()) {
    _pointer_val = QPointF(_F[idx], _dBm[idx]);
    _pointer_pos = QPoint((_F[idx]-_Fmin)/dx, height()-(_dBm[idx]-_dBmMin)/dy);
    _pointer = true;
    LogMessage msg(LOG_DEBUG);
    msg << "Place pointer at idx " << idx
        << " @ " << _pointer_pos.x() << ", " << _pointer_pos.y()
        << " = " << _pointer_val.x() << "Hz, " << _pointer_val.y() << "dBm";
    Logger::get().log(msg);
  } else {
    _pointer = false;
  }
  update();
}

