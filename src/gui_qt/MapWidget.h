#ifndef AW_QT_MAP_WIDGET_H 
	#define AW_QT_MAP_WIDGET_H

#include <QWidget>
#include <QImage>

class MapWidget: public QWidget {
	Q_OBJECT

	public:
		MapWidget(QWidget* parent);

	protected:
		virtual void paintEvent(QPaintEvent* event);

	private:
		QImage backgroundImage;
};

#endif
