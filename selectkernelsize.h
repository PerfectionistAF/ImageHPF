#ifndef SELECTKERNELSIZE_H
#define SELECTKERNELSIZE_H

#include <QDialog>

namespace Ui {
class SelectKernelSize;
}

class SelectKernelSize : public QDialog
{
    Q_OBJECT

public:
    explicit SelectKernelSize(QWidget *parent = nullptr);
    ~SelectKernelSize();

private:
    Ui::SelectKernelSize *ui;
};

#endif // SELECTKERNELSIZE_H
