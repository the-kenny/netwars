#include <QImage>
#include <QThread>
#include <QPainter>

class MinimapDrawingThread : public QThread {
  Q_OBJECT

  Q_SIGNALS:
  void minimapReady(QImage img);


public:
  void run();
};
