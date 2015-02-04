#ifndef ADDPROFILEDIALOGUE_H
#define ADDPROFILEDIALOGUE_H

#include <QDialog>
#include <QFutureWatcher>

namespace Ui {
class AddProfileDialogue;
}

class AddProfileDialogue : public QDialog
{
    Q_OBJECT

public:
    explicit AddProfileDialogue(bool _enforce, QWidget *parent = 0);
    ~AddProfileDialogue();

signals:
    void inputAccepted(const QString &name, bool usingSSURI, const QString &ssuri);
    void inputRejected(const bool enforce);

private:
    Ui::AddProfileDialogue *ui;
    const bool enforce;
    bool validName;
    bool validURI;
    QFutureWatcher<void> *fw;

private slots:
    void onProfileNameChanged(const QString &name);
    void onScanButtonClicked();
    void checkBase64SSURI(const QString &str);
    void checkIsValid();
    void onAccepted();
    void onRejected();
};

#endif // ADDPROFILEDIALOGUE_H
