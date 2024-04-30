
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle(defaultWindowTitle);
    //ui->imageView->setScene(sceneManager.scene());

    //connect();//connect to next and previous image methods
    mResourceDir = "../../../Resources";
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*void MainWindow:: openImage(const QString& filePath)
{
    if(sceneManager.isFileSupported(filePath))
    {
        sceneManager.openImage(filePath);
        fitInView();
    }
    updateButtons();
}

void MainWindow:: onImageChanged(QString &filePath)
{

}

void MainWindow:: fitInView()
{

}

void MainWindow:: showOpenDialog()
{

}

void MainWindow:: updateButtons()
{

}
*/
void MainWindow::on_filter_clicked()
{
    //this->hide();  //hide current mainwindow.ui
    //open the select kernel page
    SelectKernelSize kernel;
    kernel.setModal(true);
    kernel.exec();
}


void MainWindow::on_upload_clicked()
{
}

