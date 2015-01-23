#ifndef QRCODEDIALOGUE_H
#define QRCODEDIALOGUE_H

#include <QDialog>

namespace Ui {
class ShareDialogue;
}

class ShareDialogue : public QDialog
{
    Q_OBJECT

public:
    explicit ShareDialogue(const QByteArray &ssUrl, QWidget *parent = 0);
    ~ShareDialogue();

private:
    Ui::ShareDialogue *ui;

private slots:
    void onSaveButtonClicked();
};

#endif // QRCODEDIALOGUE_H
