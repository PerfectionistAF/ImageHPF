#pragma once

#include <QObject>
#include "imageupload.h"

class QGraphicsScene;

class SceneManager : public QObject
{
    Q_OBJECT
public:
    explicit SceneManager(QObject *parent = nullptr);

    QGraphicsScene* scene();
    void openImage(const QString& filePath);

    void fitInView();
    bool isFileSupported(const QString& filePath);

signals:
    void imageChanged(const QString& fileName);

private:
    void showImage(const QString& filePath);

private:
    ImageUpload uploads;
    QGraphicsScene* imageScene = nullptr;
    const QStringList imgExtensions = {"*.png", "*.jpg", "*.jpeg"};
};

