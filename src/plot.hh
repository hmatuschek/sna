#ifndef PLOT_HH
#define PLOT_HH

#include <QWidget>
#include <vector>
#include <list>
#include <QPainter>
#include <QPoint>
#include <QList>
#include <QColor>


/** Trivial container that contains a graph. */
class Graph
{
public:
  Graph();
  Graph(const std::vector<double> &x, const std::vector<double> &y);
  Graph(const Graph &other);

  inline const std::vector<double> &x() const { return _x; }
  inline const std::vector<double> &y() const { return _y; }

protected:
  std::vector<double> _x;
  std::vector<double> _y;
};


/** A trivial plot widget. */
class Plot : public QWidget
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit Plot(QWidget *parent = 0);

public slots:
  /** Plots the given data. */
  void plot(const std::vector<double> &F, const std::vector<double> &dBm);
  /** Plots the given data. */
  void plot(const Graph &graph);
  /** Update plot. */
  void updatePlot();
  /** Clear plot. */
  void clear();

protected:
  /** Draws the plot. */
  void paintEvent(QPaintEvent *evt);
  /** Draws the graph. */
  void _drawGraphs(QPainter &painter);
  /** Draws the axes. */
  void _drawAxes(QPainter &painter);
  /** Draws the cursor/pointer. */
  void _drawPointer(QPainter &painter);
  /** Mouse click event handler. */
  void mouseReleaseEvent(QMouseEvent *evt);

  /** Frequency range. */
  double _Xmin, _Xmax;
  double _PXmin, _PXmax;
  /** Power range. */
  double _Ymin, _Ymax;
  double _PYmin, _PYmax;
  /** Graphs. */
  std::list<Graph> _graphs;

  /** If @c true, the cursor will be drawn. */
  bool _pointer;
  /** Cursor position. */
  QPoint _pointer_pos;
  /** Cursor value. */
  QPointF _pointer_val;

public:
  static QList<QColor> _defaultColors;
};

#endif // PLOT_HH
