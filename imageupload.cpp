#include "imageupload.h"
//#include <QString>
#include <QFileInfo>
#include <QDir>

ImageUpload::ImageUpload()
{

}
void ImageUpload::setExtensions(const QStringList& extensions)
{
    supportedExtensions = extensions;
}
void ImageUpload:: openFile(const QString& filePath)
{
    QFileInfo current(filePath);
    QDir directory = current.absoluteDir();
    currentFilePath = filePath;
    currentFileName = current.fileName();

    fileNamesInCurrentPath = dir.entryList(supportedExtensions, QDir::ImageUpload, QDir::Name);
    int index = fileNamesInCurrentPath.indexOf(currentFileName);
    //hasPrevious = index !=0 ? true:false;
    //hasNext = index != fileNamesInCurrentPath.size() - 1? true:false;
}

/*QString ImageUpload:: nextFile()
{
    printf("NextFile");
}
QString ImageUpload:: previousFile()
{

}

bool ImageUpload:: hasNextFile()
{

}
bool ImageUpload:: hasPreviousFile()
{

}
*/
QString ImageUpload:: getCurrentFileName()
{
    return currentFileName;
}

