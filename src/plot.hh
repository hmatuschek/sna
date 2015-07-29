#ifndef PLOT_HH
#define PLOT_HH

#include <QWidget>
#include <vector>
#include <QPainter>
#include <QPoint>

class Plot : public QWidget
{
  Q_OBJECT
public:
  explicit Plot(QWidget *parent = 0);

public slots:
  void plot(const std::vector<double> &F, const std::vector<double> &dBm, size_t N);

protected:
  void paintEvent(QPaintEvent *evt);
  void _drawGraph(QPainter &painter);
  void _drawAxes(QPainter &painter);
  void _drawPointer(QPainter &painter);
  void mouseReleaseEvent(QMouseEvent *evt);

  double _Fmin, _Fmax;
  double _dBmMin, _dBmMax;
  std::vector<double> _F;
  std::vector<double> _dBm;

  bool _pointer;
  QPoint _pointer_pos;
  QPointF _pointer_val;
};

#endif // PLOT_HH
