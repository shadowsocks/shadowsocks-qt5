#ifndef QRCODECAPTURER_H
#define QRCODECAPTURER_H

#include <QDialog>

namespace Ui {
class QRCodeCapturer;
}

class QRCodeCapturer : public QDialog
{
    Q_OBJECT

public:
    explicit QRCodeCapturer(QWidget *parent = 0);
    ~QRCodeCapturer();

    static QString scanEntireScreen();

signals:
    void qrCodeFound(const QString &result);

protected slots:
    void moveEvent(QMoveEvent *e);
    void resizeEvent(QResizeEvent *e);

private:
    Ui::QRCodeCapturer *ui;
    void decodeCurrentRegion();
};

#endif // QRCODECAPTURER_H
