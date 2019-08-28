#ifndef VIDEODEFS_H
#define VIDEODEFS_H
#include <QString>
struct VideoSource
{
    QString fileName;
    qint32 frameCount;
    qint32 duration;
    qint32 entryPoint;
};
#endif // VIDEODEFS_H
