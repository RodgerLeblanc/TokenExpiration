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

#include "applicationui.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>
#include <bb/system/InvokeManager>

#include <QTimer>

using namespace bb::cascades;
using namespace bb::system;

ApplicationUI::ApplicationUI() :
        QObject(),
        m_translator(new QTranslator(this)),
        m_localeHandler(new LocaleHandler(this)),
        m_invokeManager(new InvokeManager(this)),
        m_ppsWatch(new PpsWatch("/pps/system/development/devmode", this))
{
    InvokeRequest request;
    request.setTarget("com.CellNinja.TokenExpirationService");
    request.setAction("com.CellNinja.TokenExpirationService.START");
    m_invokeManager->invoke(request);

    connect(m_ppsWatch, SIGNAL(ppsFileReady(const QVariantMap&)), this, SLOT(onPpsFileReady(const QVariantMap&)));

    // prepare the localization
    if (!QObject::connect(m_localeHandler, SIGNAL(systemLanguageChanged()),
            this, SLOT(onSystemLanguageChanged()))) {
        // This is an abnormal situation! Something went wrong!
        // Add own code to recover here
        qWarning() << "Recovering from a failed connect()";
    }

    // initial load
    onSystemLanguageChanged();

    qDebug() << "Registering QTimer for easy QML access";
    qmlRegisterType<QTimer>("QTimer", 1, 0, "QTimer");

    // Create scene document from main.qml asset, the parent is set
    // to ensure the document gets destroyed properly at shut down.
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

    // Make app available to the qml.
    qml->setContextProperty("_app", this);

    // Create root object for the UI
    AbstractPane *root = qml->createRootObject<AbstractPane>();

    // Set created root object as the application scene
    Application::instance()->setScene(root);
}

void ApplicationUI::onPpsFileReady(const QVariantMap& map) {
    if (map["debug_token_installed"].toBool()) {
        QString dateTime = map["debug_token_expiration"].toString();
        int start = dateTime.lastIndexOf(":") + 2;
        int length = dateTime.lastIndexOf(" ") - start;
        const QDateTime expirationDateTime = QDateTime::fromString(dateTime.remove(start, length));
        if (!expirationDateTime.isValid()) { qDebug() << "Invalid date"; return; }

        emit expirationDateTimeChanged(QDateTime::fromString(dateTime).toString("yyyy/MM/dd hh:mm"));
    }
    else {
        emit debugTokenNotInstalled();
    }
}

void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(m_translator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale().name();
    QString file_name = QString("TokenExpiration_%1").arg(locale_string);
    if (m_translator->load(file_name, "app/native/qm")) {
    QCoreApplication::instance()->installTranslator(m_translator);
    }
}
