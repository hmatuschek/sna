#include "plot.hh"
#include <inttypes.h>
#include <QPainter>
#include <QPaintEvent>
#include <QFont>
#include <QString>
#include <QRectF>
#include <cmath>
#include "logger.hh"



/* ******************************************************************************************** *
 * Implementation of Graph
 * ******************************************************************************************** */
Graph::Graph()
    : _x(), _y()
{
    // pass...
}

Graph::Graph(const std::vector<double> &x, const std::vector<double> &y)
    : _x(x), _y(y)
{
    // pass...
}

Graph::Graph(const Graph &other)
    : _x(other._x), _y(other._y)
{
    // pass...
}


/* ******************************************************************************************** *
 * Implementation of Plot
 * ******************************************************************************************** */
QList<QColor> Plot::_defaultColors = QList<QColor>()
    << QColor(0, 0, 125) << QColor(125, 0, 0) << QColor(0, 125, 0) << QColor(125, 125, 0)
    << QColor(0, 125, 125) << QColor(125, 0, 125) << QColor(205, 79, 18) << QColor(255, 185, 24)
    << QColor(243, 250, 146) << QColor(105, 151, 102) << QColor(69, 47, 96)
    << QColor(224, 26, 53) << QColor(204, 15, 19) << QColor(63, 61, 153) << QColor(153, 61, 113)
    << QColor(61, 153, 86) << QColor(61, 90, 153) << QColor(153, 61, 144) << QColor(61, 121, 153)
    << QColor(132, 61, 153) << QColor(153, 78, 61) << QColor(98, 153, 61) << QColor(61, 151, 153)
    << QColor(101, 61, 153) << QColor(153, 61, 75);

Plot::Plot(QWidget *parent)
  : QWidget(parent), _Xmin(0), _Xmax(0), _PXmin(0), _PXmax(0),
    _Ymin(0), _Ymax(0), _PYmin(0), _PYmax(0), _graphs(), _pointer(false)
{
  setMinimumSize(640, 480);
}

void
Plot::plot(const Graph &graph) {
  _graphs.push_back(graph);
  updatePlot();
}

void
Plot::plot(const std::vector<double> &F, const std::vector<double> &dBm) {
  _graphs.push_back(Graph(F, dBm));
  updatePlot();
}

void
Plot::updatePlot() {
  _Xmin = _Xmax = _graphs.front().x()[0];
  _Ymin = _Ymax = _graphs.front().y()[0];
  std::list<Graph>::iterator graph = _graphs.begin();
  for (; graph != _graphs.end(); graph++) {
    size_t N = graph->x().size();
    for (size_t i=0; i<N; i++) {
      _Xmin = std::min(_Xmin, graph->x()[i]);
      _Xmax = std::max(_Xmax, graph->x()[i]);
      _Ymin = std::min(_Ymin, graph->y()[i]);
      _Ymax = std::max(_Ymax, graph->y()[i]);
    }
  }
  _PXmin = _Xmin; _PXmax = _Xmax;
  _PYmin = std::floor(_Ymin - 0.05*std::abs(_Ymax-_Ymin));
  _PYmax = std::ceil(_Ymax + 0.05*std::abs(_Ymax-_Ymin));
  // Force redraw
  update();
}

void
Plot::clear() {
  _graphs.clear();
  _pointer = false;
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

  _drawGraphs(painter);
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
  double dF = (_PXmax-_PXmin)/8;
  double F  = _PXmin;

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

  double dv = (_PYmax-_PYmin)/8, v  = _PYmax-dv;
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
Plot::_drawGraphs(QPainter &painter) {
  painter.save();

  size_t idx = 0;
  QPen pen(_defaultColors.at(idx));
  idx = (idx+1) % _defaultColors.size();
  pen.setWidth(2);
  pen.setStyle(Qt::SolidLine);
  painter.setPen(pen);

  double ppx = width()/(_PXmax-_PXmin);
  double ppy = height()/(_PYmax-_PYmin);
  std::list<Graph>::iterator graph = _graphs.begin();
  for (; graph != _graphs.end(); graph++) {
    size_t N = graph->x().size();
    for (size_t j=1; j<N; j++) {
      int x1 = ppx*(graph->x()[j-1]-_PXmin);
      int y1 = height()-ppy*(graph->y()[j-1]-_PYmin);
      int x2 = ppx*(graph->x()[j]-_PXmin);
      int y2 = height()-ppy*(graph->y()[j]-_PYmin);
      painter.drawLine(x1,y1, x2,y2);
    }
    pen.setColor(_defaultColors.at(idx));
    painter.setPen(pen);
    idx = (idx+1) % _defaultColors.size();
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
                   _pointer_pos.x(), _pointer_pos.y());
  painter.drawLine(_pointer_pos.x(), _pointer_pos.y(),
                   _pointer_pos.x()-5,_pointer_pos.y()-5);

  QString label;
  if (_pointer_val.x()<10e3) {
    label = tr("%1 dBm @%2 Hz").arg(QString::number(_pointer_val.y(), 'f', 1)).arg(QString::number(_pointer_val.x(), 'f', 1));
  } else {
    label = tr("%1 dBm @%2 kHz").arg(QString::number(_pointer_val.y(), 'f', 1)).arg(QString::number(_pointer_val.x()/1e3, 'f', 2));
  }
  painter.drawText(_pointer_pos.x()+10, _pointer_pos.y()-10, label);
  painter.restore();
}

void
Plot::mouseReleaseEvent(QMouseEvent *evt) {
  QWidget::mouseReleaseEvent(evt);
  double dx = (_PXmax-_PXmin)/width();
  double dy = (_PYmax-_PYmin)/height();
  // Update pointer position
  _pointer_pos = QPoint(evt->pos());
  _pointer_val = QPointF(_pointer_pos.x()*dx + _PXmin, (height()-_pointer_pos.y())*dy + _PYmin);
  _pointer = true;
  // log message
  /*LogMessage msg(LOG_DEBUG);
  msg << "Place pointer @ " << _pointer_pos.x() << ", " << _pointer_pos.y()
      << " = " << _pointer_val.x() << "Hz, " << _pointer_val.y() << "dBm";
  Logger::get().log(msg); */
  update();
}

