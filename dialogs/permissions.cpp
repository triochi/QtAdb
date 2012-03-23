#include "permissions.h"
#include "ui_permissions.h"

PermissionsDialog::PermissionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PermissionsDialog)
{
    *((unsigned short*)&permissions) = 0;
    ui->setupUi(this);
}

PermissionsDialog::~PermissionsDialog()
{
    delete ui;
}

void PermissionsDialog::setPermissions(unsigned short perm)
{
    *((unsigned short*)&permissions) = perm;
    ui->p0->setChecked(permissions.b0);
    ui->p1->setChecked(permissions.b1);
    ui->p2->setChecked(permissions.b2);
    ui->p3->setChecked(permissions.b3);
    ui->p4->setChecked(permissions.b4);
    ui->p5->setChecked(permissions.b5);
    ui->p6->setChecked(permissions.b6);
    ui->p7->setChecked(permissions.b7);
    ui->p8->setChecked(permissions.b8);
}

void PermissionsDialog::on_p8_toggled(bool checked)
{
    permissions.b8 = checked;
}

void PermissionsDialog::on_p7_toggled(bool checked)
{
    permissions.b7 = checked;
}

void PermissionsDialog::on_p6_toggled(bool checked)
{
    permissions.b6 = checked;
}

void PermissionsDialog::on_p5_toggled(bool checked)
{
    permissions.b5 = checked;
}

void PermissionsDialog::on_p4_toggled(bool checked)
{
    permissions.b4 = checked;
}

void PermissionsDialog::on_p3_toggled(bool checked)
{
    permissions.b3 = checked;
}

void PermissionsDialog::on_p2_toggled(bool checked)
{
    permissions.b2 = checked;
}

void PermissionsDialog::on_p1_toggled(bool checked)
{
    permissions.b1 = checked;
}

void PermissionsDialog::on_p0_toggled(bool checked)
{
    permissions.b0 = checked;
}

void PermissionsDialog::on_buttonBox_accepted()
{
    this->setResult(1);
}
