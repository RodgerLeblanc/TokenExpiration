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

#include "service.hpp"

#include <bb/Application>
#include <bb/platform/Notification>
#include <bb/platform/NotificationDefaultApplicationSettings>
#include <bb/system/InvokeManager>
#include <bb/system/InvokeTimerRequest>
#include <bb/system/InvokeReply>

#include <QSettings>
#include <QTimer>

using namespace bb::platform;
using namespace bb::system;

Service::Service() :
        QObject(),
        m_notify(new Notification(this)),
        m_invokeManager(new InvokeManager(this)),
        m_ppsWatch(new PpsWatch("/pps/system/development/devmode", this))
{
    m_invokeManager->connect(m_invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)),
            this, SLOT(handleInvoke(const bb::system::InvokeRequest&)));

    NotificationDefaultApplicationSettings settings;
    settings.setPreview(NotificationPriorityPolicy::Allow);
    settings.apply();

    connect(m_ppsWatch, SIGNAL(ppsFileReady(const QVariantMap&)), this, SLOT(onPpsFileReady(const QVariantMap&)));
}

void Service::checkTokenExpiracy() {
    bb::PpsObject ppsObject("/pps/system/development/devmode");
    if (ppsObject.open( bb::PpsOpenMode::Subscribe )) {
        bool readOk;
        QByteArray data = ppsObject.read(&readOk);
        if(!readOk) { return; }

        bool decodeOk;
        const QVariantMap map = bb::PpsObject::decode(data, &decodeOk);
        if(!decodeOk) { return; }

        const QVariantMap ppsFile = map["@devmode"].toMap();
        if (ppsFile.isEmpty()) { return; }

        this->onPpsFileReady(ppsFile);
    }
}

void Service::handleInvoke(const bb::system::InvokeRequest & request)
{
    qDebug() << "handleInvoke" << request.action();
    if (request.action().compare("bb.action.system.TIMER_FIRED") == 0) {
        this->triggerNotification();
    }
    else {
        this->checkTokenExpiracy();
    }
}

void Service::invalidTokenExpiration() {
    qDebug() << "Invalid date";

    Notification::clearEffectsForAll();
    Notification::deleteAllFromInbox();

    m_notify->setTitle("The token expiration could not be retrieved.");
    m_notify->setBody("You may not have any token installed, please update your token in Momentics. This app will continue running (headless) until you update your token.");

    bb::system::InvokeRequest request;
    request.setTarget("com.CellNinja.TokenExpiration");
    request.setAction("bb.action.START");
    m_notify->setInvokeRequest(request);

    m_notify->notify();
}

void Service::onFinished() {
    qDebug() << "onFinished()" << m_invokeReply->errorCode() << m_invokeReply->error();

    if ((m_invokeReply->errorCode() == 0) || (m_invokeReply->error() == bb::system::InvokeReplyError::None)) {
        bb::Application::instance()->quit();
    }
    else {
        qDebug() << "There was an error, check back in an hour, when things will have cool down ;)";
        QTimer::singleShot((1000 * 60 * 60), this, SLOT(checkTokenExpiracy()));
    }
}

void Service::onPpsFileReady(const QVariantMap& map) {
    qDebug() << "onPpsFileReady():" << map;

    QString dateTime = map["debug_token_expiration"].toString();
    int start = dateTime.lastIndexOf(":") + 2;
    int length = dateTime.lastIndexOf(" ") - start;
    const QDateTime expirationDateTime = QDateTime::fromString(dateTime.remove(start, length));
    if (!expirationDateTime.isValid()) { this->invalidTokenExpiration(); }

    QSettings settings;
    settings.setValue("expirationDateTime", expirationDateTime);
    this->setTimerTrigger(expirationDateTime);
}

void Service::triggerNotification()
{
    Notification::clearEffectsForAll();
    Notification::deleteAllFromInbox();

    QSettings settings;
    QDateTime dateTime = settings.value("expirationDateTime", QDateTime::currentDateTime().addDays(2)).toDateTime();

    if (QDateTime::currentDateTime().msecsTo(dateTime) < 0) {
        m_notify->setTitle("Your Debug Token is expired.");
        m_notify->setBody("Expiration: " + dateTime.toString() + "\n\nThis app will continue running (headless) until you update your token.");

        bb::system::InvokeRequest request;
        request.setTarget("com.CellNinja.TokenExpiration");
        request.setAction("bb.action.START");
        m_notify->setInvokeRequest(request);
    }
    else {
        m_notify->setTitle("Your Debug Token is about to expire.");
        m_notify->setBody("Expiration: " + dateTime.toString());
    }

    m_notify->notify();
}

void Service::setTimerTrigger(const QDateTime& expirationDateTime) {
    int daysToExpiration = QDateTime::currentDateTime().daysTo(expirationDateTime);

    if (daysToExpiration <= 0) {
        this->triggerNotification();
        // Keep running headless to get notified when token is updated
        return;
    }

    int daysBefore = (daysToExpiration > 2) ? 2 : 1;
    QDateTime notificationDateTime = expirationDateTime.addDays(-daysBefore);
    qDebug() << "Timer trigger set to" << notificationDateTime.toString("yyyy/MM/dd hh:mm");

    // Set the timer trigger for 2 days before expiration
    InvokeDateTime trigdatetime(notificationDateTime.date(), notificationDateTime.time(), "");
    qDebug() << trigdatetime.date() << trigdatetime.time();

    // Create the timer request
    InvokeTimerRequest timer_request("debugTokenTrigger", trigdatetime, "com.CellNinja.TokenExpirationService");

    m_invokeManager->deregisterTimer("debugTokenTrigger");
    m_invokeReply = m_invokeManager->registerTimer(timer_request);
    connect(m_invokeReply, SIGNAL(finished()), this, SLOT(onFinished()));
}
