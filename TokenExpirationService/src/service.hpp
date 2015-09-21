/*
 * Copyright (c) 2013-2015 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SERVICE_H_
#define SERVICE_H_

#include <src/PpsWatch/PpsWatch.h>

#include <QObject>
#include <QDebug>
#include <QDateTime>

namespace bb {
    class Application;
    namespace platform {
        class Notification;
    }
    namespace system {
        class InvokeTimerRequest;
        class InvokeManager;
        class InvokeReply;
        class InvokeRequest;
    }
}

class Service: public QObject
{
    Q_OBJECT
public:
    Service();
    virtual ~Service() { qDebug() << "HL is done..."; }

private slots:
    void checkTokenExpiracy();
    void handleInvoke(const bb::system::InvokeRequest &);
    void onFinished();
    void onPpsFileReady(const QVariantMap&);

private:
    void invalidTokenExpiration();
    void setTimerTrigger(const QDateTime& expirationDateTime);
    void triggerNotification();

    bb::platform::Notification * m_notify;
    bb::system::InvokeManager * m_invokeManager;
    bb::system::InvokeReply* m_invokeReply;

    PpsWatch* m_ppsWatch;
};

#endif /* SERVICE_H_ */
