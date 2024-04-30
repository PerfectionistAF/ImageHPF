
#include <QMainWindow>
#include "selectkernelsize.h"
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

//class SceneManager;

class MainWindow : public QMainWindow

{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //void openImage(const QString& filePath);

private slots:
    void on_filter_clicked();

    void on_upload_clicked();

    /*void onImageChanged(QString &filePath);
    void fitInView();
    void showOpenDialog();
    void updateButtons();*/


private:
    Ui::MainWindow *ui;
    QString mResourceDir;
    //SceneManager& sceneManager;
    const QString defaultWindowTitle = "ImageHPF";
};


