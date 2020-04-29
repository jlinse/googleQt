#pragma once

#include <QObject>
#include "google/endpoint/ApiUtil.h"

namespace googleQt{
    namespace demo{
        class GOOGLEQT_DLLSPEC ApiListener: public QObject
        {
            Q_OBJECT

        public slots:
            void transferProgress(qint64 bytesProcessed, qint64 total);
        };
    }//demo
};//googleQt
