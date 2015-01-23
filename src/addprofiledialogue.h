#ifndef ADDPROFILEDIALOGUE_H
#define ADDPROFILEDIALOGUE_H

#include <QDialog>

namespace Ui {
class AddProfileDialogue;
}

class AddProfileDialogue : public QDialog
{
    Q_OBJECT

public:
    explicit AddProfileDialogue(QWidget *parent = 0, bool e = false);
    ~AddProfileDialogue();

signals:
    void inputAccepted(const QString &, bool = false, const QString & = 0);
    void inputRejected(bool);

private:
    Ui::AddProfileDialogue *ui;
    bool enforce;

private slots:
    void onScanButtonClicked();
    void checkBase64SSURI(const QString &str);
    void onAccepted();
    void onRejected();
};

#endif // ADDPROFILEDIALOGUE_H
