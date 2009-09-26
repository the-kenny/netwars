#include <QImage>
#include <QThread>
#include <QPainter>
#include <QLabel>
#include <string>

class MinimapDrawingThread : public QThread {
  Q_OBJECT

  public:
  MinimapDrawingThread(const std::string& mapFile,
					   QLabel* label);
  void run();

  Q_SIGNALS:
  void minimapReady(QImage img);

private:
  std::string mapFile_;
  QLabel* label_;
};
