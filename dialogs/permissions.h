#ifndef PERMISSIONS_H
#define PERMISSIONS_H


#include <QDialog>
#include <QCryptographicHash>
#include <QSettings>

typedef struct {
    bool b0:1;
    bool b1:1;
    bool b2:1;
    bool b3:1;
    bool b4:1;
    bool b5:1;
    bool b6:1;
    bool b7:1;
    bool b8:1;
} Permissions;

namespace Ui {
    class PermissionsDialog;

}

class PermissionsDialog : public QDialog
{
    Q_OBJECT

public:
    PermissionsDialog(QWidget *parent);
    ~PermissionsDialog();
    void setPermissions(unsigned short perm);
    unsigned short getPermissions(){return *((unsigned short *)&permissions);}
private slots:
private:
    Ui::PermissionsDialog *ui;
    Permissions permissions;

private slots:
    void on_buttonBox_accepted();
    void on_p0_toggled(bool checked);
    void on_p1_toggled(bool checked);
    void on_p2_toggled(bool checked);
    void on_p3_toggled(bool checked);
    void on_p4_toggled(bool checked);
    void on_p5_toggled(bool checked);
    void on_p6_toggled(bool checked);
    void on_p7_toggled(bool checked);
    void on_p8_toggled(bool checked);
};


#endif // PERMISSIONS_H
