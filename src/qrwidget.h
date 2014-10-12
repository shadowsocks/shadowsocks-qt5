#ifndef QRWIDGET_H
#define QRWIDGET_H

#include <QWidget>
#include <QPaintEvent>

class QRWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QRWidget(QWidget *parent = 0);
    void setQRData(const QByteArray &qrData);

private:
    QByteArray data;

protected:
    void paintEvent(QPaintEvent *);
};

#endif // QRWIDGET_H
