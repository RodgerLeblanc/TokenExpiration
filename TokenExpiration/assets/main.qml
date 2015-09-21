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

import bb.cascades 1.3

Page {
    property bool debugTokenInstalled: true
    property string tokenExpiration
    
    Menu.definition: MenuDefinition {
        actions: [
            ActionItem {
                title: qsTr("About") + Retranslate.onLocaleOrLanguageChanged
                ActionBar.placement: ActionBarPlacement.OnBar
                imageSource: "asset:///images/ic_info.png"
                onTriggered: { aboutSheet.open() }
            }
        ]      
    }
    
    attachedObjects: [
        AboutSheet { id: aboutSheet },
        ComponentDefinition {
            id: activeFrame
            source: "asset:///cover/AppCover.qml"
        }
    ]
    
    onCreationCompleted: {
        // Creates a MultiCover or SceneCover depending on asset selection
        Application.setCover(activeFrame.createObject())
        _app.debugTokenNotInstalled.connect(onDebugTokenNotInstalled)
        _app.expirationDateTimeChanged.connect(onExpirationDateTimeChanged)
    }
    
    function onDebugTokenNotInstalled() {
        debugTokenInstalled = false
    }
    
    function onExpirationDateTimeChanged(date) {
        tokenExpiration = date
    }
    
    ScrollView {
        Container {
            layout: DockLayout {}
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            Container {
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
                leftPadding: ui.du(3)
                rightPadding: leftPadding
                topPadding: leftPadding
                bottomPadding: leftPadding
                Label {
                    visible: debugTokenInstalled
                    text: qsTr("Debug Token Expiration: ") + Retranslate.onLocaleOrLanguageChanged + tokenExpiration
                    multiline: true
                    horizontalAlignment: HorizontalAlignment.Center
                    textStyle.fontSize: FontSize.Large
                    textStyle.fontWeight: FontWeight.Bold
                    textStyle.color: Color.Red
                }
                Label {
                    visible: !debugTokenInstalled
                    text: qsTr("There is no debug token installed on this device. Please open Momentics on your desktop and update your debug token. (This app is targeted at developers only, if you don't know what is a debug token, you don't need this app)") + Retranslate.onLocaleOrLanguageChanged + tokenExpiration
                    multiline: true
                    horizontalAlignment: HorizontalAlignment.Center
                    textStyle.fontSize: FontSize.Large
                    textStyle.fontWeight: FontWeight.Bold
                    textStyle.color: Color.Red
                }
                Divider {}
                Label {
                    text: qsTr("This app will notify you in hub 2 days and one day before your debug token expires.") + Retranslate.onLocaleOrLanguageChanged
                    multiline: true
                    horizontalAlignment: HorizontalAlignment.Center
                    textStyle.textAlign: TextAlign.Justify
                    textStyle.fontSize: FontSize.Large
                }
                Divider {}
                Label {
                    text: qsTr("This app is headless but it will (usually) not run in the background. It uses voodoo magic and incantation to wake itself up automatically (it's also called Timer Trigger and was introduced in 10.3). You can find the source code of this app at" + Retranslate.onLocaleOrLanguageChanged + " https://github.com/RodgerLeblanc/TokenExpiration")
                    multiline: true
                    content.flags: TextContentFlag.ActiveText
                    horizontalAlignment: HorizontalAlignment.Center
                    textStyle.textAlign: TextAlign.Justify
                    textStyle.fontSize: FontSize.Large
                }
            }
        }
    }
}
