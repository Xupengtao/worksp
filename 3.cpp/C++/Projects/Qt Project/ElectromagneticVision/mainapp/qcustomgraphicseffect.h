#ifndef QCUSTOMGRAPHICSEFFECT_H
#define QCUSTOMGRAPHICSEFFECT_H

#include <QObject>
#include <QGraphicsEffect>
#include <QPainter>

class QCustomGraphicsEffect : public QGraphicsEffect
{
    Q_OBJECT
public:
    explicit QCustomGraphicsEffect(QObject *parent = nullptr) : QGraphicsEffect(parent)
    {

    }

protected:
    void draw(QPainter *painter)
    {
        QImage image;
        image = sourcePixmap().toImage();
        image = image.convertToFormat(QImage::Format_Grayscale8);
        for(int i=0; i<image.byteCount(); i++)
        {
            image.bits()[i] = image.bits()[i] < 100 ? 0 : 255;
        }
        painter->drawPixmap(0,0,QPixmap::fromImage(image));
    }

signals:

public slots:
};

#endif // QCUSTOMGRAPHICSEFFECT_H
