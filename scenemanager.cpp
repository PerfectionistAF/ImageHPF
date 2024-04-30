
#include "scenemanager.h"
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QImageReader>

SceneManager::SceneManager(QObject *parent)
    : QObject{parent}
{
    imageScene = new QGraphicsScene(this);
    uploads.setExtensions(imgExtensions);
}

QGraphicsScene* SceneManager:: scene()
{
    return imageScene;
}

void SceneManager:: openImage(const QString& filePath)
{
    uploads.openFile(filePath);
    showImage(filePath);
}

void SceneManager:: fitInView()
{
    if(imageScene->items().size() && imageScene->views().size())
    {
        auto view = imageScene->views().at(0);
        view->fitInView(imageScene->items().at(0), Qt::KeepAspectRatio);
        view->setSceneRect(imageScene->items().at(0)->boundingRect());
    }
}
bool SceneManager:: isFileSupported(const QString& filePath)
{
    for(auto ext : imgExtensions)
    {
        ext = ext.remove(0, 1);
        if(filePath.endsWith(ext, Qt::CaseInsensitive))
        {
            return true;
        }
    }
    return false;
}

void SceneManager:: showImage(const QString& filePath)
{
    imageScene->clear();
    QImageReader imgReader(filePath);
    imgReader.setAutoTransform(true);

    QImage img = imgReader.read();
    if(!img.isNull())
    {
        QPixmap pixmap = QPixmap ::fromImage(img);
        imageScene->addPixmap(pixmap);
        imageScene->update();

        emit imageChanged(uploads.getCurrentFileName());
    }
}

