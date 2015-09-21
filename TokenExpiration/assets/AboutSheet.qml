import bb.cascades 1.3
import bb.system 1.2
import "MyApps"

Sheet {
    id: aboutSheet
    
    onOpened: { Application.setMenuEnabled(false); myApps.autoScroll = true; }
    onClosed: { Application.setMenuEnabled(true); myApps.autoScroll = false; }
    
    Page {
        titleBar: TitleBar {
            title: qsTr("About") + Retranslate.onLanguageChanged
            acceptAction: ActionItem {
                title: qsTr("Ok") + Retranslate.onLanguageChanged
                onTriggered: { aboutSheet.close() }
            }
        }
        
        ScrollView {
            Container {
                id: mainContainer
                
                layout: DockLayout {}
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                leftPadding: 30
                rightPadding: leftPadding
                topPadding: 10
                bottomPadding: topPadding
                Container {
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Center
                    Label {
                        id: appNameLabel
                        horizontalAlignment: HorizontalAlignment.Center
                        text: Application.applicationName + " " + Application.applicationVersion
                        textStyle.base: SystemDefaults.TextStyles.BigText
                    }
                    Label {
                        property string emailUri: "mailto:roger@rogerleblanc.net?subject=" + appNameLabel.text + " support"
                        horizontalAlignment: HorizontalAlignment.Center
                        text: "<html><a href=\"" + emailUri + "\">" + qsTr("by CellNinja") + Retranslate.onLanguageChanged + "</a></html>"
                        multiline: true
                        textStyle.base: SystemDefaults.TextStyles.TitleText
                    }
                    MyApps { id: myApps; topPadding: ui.du(3); }
                    Container {
                        topPadding: 30
                        horizontalAlignment: HorizontalAlignment.Center
                        Button {
                            text: qsTr("CellNinja BBM Channel") + Retranslate.onLanguageChanged
                            imageSource: (Application.themeSupport.theme.colorTheme.style == VisualStyle.Dark) ? "asset:///images/ic_bbmchannel.png" : "asset:///images/ic_bbmchannel_black.png"
                            onClicked: { 
                                invokeChannel.query.invokeTargetId = "sys.bbm.channels.card.previewer"
                                invokeChannel.query.invokeActionId = "bb.action.OPENBBMCHANNEL"
                                invokeChannel.query.uri = "bbmc:C004E866D"
                                invokeChannel.query.updateQuery()
                            }
                            attachedObjects: [
                                Invocation {
                                    id: invokeChannel
                                    onArmed: { trigger(query.invokeActionId) }
                                }
                            ]
                        }
                    }
                }
            }
        }
    }
}