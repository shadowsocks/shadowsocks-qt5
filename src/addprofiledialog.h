#ifndef ADDPROFILEDIALOG_H
#define ADDPROFILEDIALOG_H

#include <QDialog>
#include <QFutureWatcher>

namespace Ui {
class AddProfileDialog;
}

class AddProfileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddProfileDialog(bool _enforce, QWidget *parent = 0);
    ~AddProfileDialog();

signals:
    void inputAccepted(const QString &name, bool usingSSURI, const QString &ssuri);
    void inputRejected(const bool enforce);

private:
    Ui::AddProfileDialog *ui;
    const bool enforce;
    bool validName;
    bool validURI;
    QFutureWatcher<void> *fw;
    QImage convertToGrey(const QImage& input);
    void setupURIfromQRImg(const QImage& qrimg);

private slots:
    void onProfileNameChanged(const QString &name);
    void onScanButtonClicked();
    void onQRFileButtonClicked();
    void checkBase64SSURI(const QString &str);
    void checkIsValid();
    void onAccepted();
    void onRejected();
};

#endif // ADDPROFILEDIALOG_H
