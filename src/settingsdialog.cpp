#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QPushButton>

SettingsDialog::SettingsDialog(ConfigHelper *ch, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog),
    helper(ch)
{
    ui->setupUi(this);

    ui->toolbarStyleComboBox->setCurrentIndex(helper->getToolbarStyle());
    ui->hideCheckBox->setChecked(helper->isHideWindowOnStartup());
    ui->oneInstanceCheckBox->setChecked(helper->isOnlyOneInstance());

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::onAccepted);
    connect(ui->toolbarStyleComboBox, &QComboBox::currentTextChanged, this, &SettingsDialog::onChanged);
    connect(ui->hideCheckBox, &QCheckBox::stateChanged, this, &SettingsDialog::onChanged);
    connect(ui->oneInstanceCheckBox, &QCheckBox::stateChanged, this, &SettingsDialog::onChanged);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    this->adjustSize();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::onAccepted()
{
    helper->setGeneralSettings(ui->toolbarStyleComboBox->currentIndex(),
                               ui->hideCheckBox->isChecked(),
                               ui->oneInstanceCheckBox->isChecked());
    this->accept();
}

void SettingsDialog::onChanged()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}
