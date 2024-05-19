#ifndef AVRECORDCONNECT_H
#define AVRECORDCONNECT_H

#include <QObject>

#include "../utils/v4l2utils.h"

class AVRecordConnect : public QObject
{
    Q_OBJECT

private:
    vector<Camera> cs;
public:
    AVRecordConnect();


public slots:
    QString getAllVideoDeviceList() const;
};

#endif // AVRECORDCONNECT_H
