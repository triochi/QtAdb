#ifndef PERMISSIONS_H
#define PERMISSIONS_H


#include <QDialog>
#include <QCryptographicHash>
#include <QSettings>

namespace Ui {
    class PermissionsDialog;
}

class PermissionsDialog : public QDialog
{
    Q_OBJECT

public:
    PermissionsDialog(QWidget *parent);
    ~PermissionsDialog();
private slots:
private:
    Ui::PermissionsDialog *ui;
};


#endif // PERMISSIONS_H
