#pragma once


class ImageUpload
{
public:
    ImageUpload();

    void setExtensions(const QStringList& extensions);
    void openFile(const QString& filePath);
    //scroll to get other files
    //QString nextFile();
    //QString previousFile();

    //bool hasNextFile();
    //bool hasPreviousFile();

    QString getCurrentFileName();

private:
    QStringList fileNamesInCurrentPath;

    QString currentFilePath;
    QString currentFileName;

    //bool hasNext = false;
    //bool hasPrevious = false;

    QStringList supportedExtensions;
};
