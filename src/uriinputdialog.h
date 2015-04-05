#ifndef URIINPUTDIALOG_H
#define URIINPUTDIALOG_H

#include <QDialog>

namespace Ui {
class URIInputDialog;
}

class URIInputDialog : public QDialog
{
    Q_OBJECT

signals:
    void acceptedURI(const QString &uri);

public:
    explicit URIInputDialog(QWidget *parent = 0);
    ~URIInputDialog();

private:
    Ui::URIInputDialog *ui;

private slots:
    void onAccepted();
    void onURIChanged(const QString &);
};

#endif // URIINPUTDIALOG_H
