#ifndef SDFILES_H
#define SDFILES_H

#include <QWidget>

namespace Ui {
class SdFiles;
}

class SdFiles : public QWidget
{
    Q_OBJECT
    
public:
    explicit SdFiles(QWidget *parent = 0);
    ~SdFiles();
    
private:
    Ui::SdFiles *ui;
};

#endif // SDFILES_H
