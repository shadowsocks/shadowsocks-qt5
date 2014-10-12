#ifndef QRCODEDIALOGUE_H
#define QRCODEDIALOGUE_H

#include <QDialog>

namespace Ui {
class QRCodeDialogue;
}

class QRCodeDialogue : public QDialog
{
    Q_OBJECT

public:
    explicit QRCodeDialogue(const QByteArray &ssUrl, QWidget *parent = 0);
    ~QRCodeDialogue();

private:
    Ui::QRCodeDialogue *ui;
};

#endif // QRCODEDIALOGUE_H
