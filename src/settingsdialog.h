#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include "confighelper.h"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(ConfigHelper *ch, QWidget *parent = 0);
    ~SettingsDialog();

private:
    Ui::SettingsDialog *ui;
    ConfigHelper *helper;

private slots:
    void onAccepted();
    void onChanged();
};

#endif // SETTINGSDIALOG_H
