#include "selectkernelsize.h"
#include "ui_selectkernelsize.h"

SelectKernelSize::SelectKernelSize(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectKernelSize)
{
    ui->setupUi(this);
}

SelectKernelSize::~SelectKernelSize()
{
    delete ui;
}
