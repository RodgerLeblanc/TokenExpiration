import bb.cascades 1.3
import QTimer 1.0

Container {
    property bool autoScroll: false
    
    onAutoScrollChanged: {
        if (autoScroll)
            moveListViewTimer.start()
        else 
            moveListViewTimer.stop()
    }
    
    CustomDivider {}    
    Label {
        text: qsTr("CellNinja Apps") + Retranslate.onLanguageChanged
        horizontalAlignment: HorizontalAlignment.Center
        multiline: true
    }
    ListView {
        id: listView
        
        property int iterator: 0
        property int itemWidth: listViewLayoutUpdateHandler.layoutFrame.width
        
        dataModel: XmlDataModel { source: qsTr("asset:///MyApps/model.xml") + Retranslate.onLanguageChanged }
        layout: StackListLayout { orientation: LayoutOrientation.LeftToRight }
        preferredHeight: ui.du(30)
        maxHeight: preferredHeight
        flickMode: FlickMode.SingleItem
        
        onTouch: {
            if (event.touchType == TouchType.Up)
                autoScroll = false
        }
        
        function du(unit) {
            return ui.du(unit)
        }
        
        onTriggered: {
            appWorldInvoke.query.setUri(dataModel.data(indexPath).link)
            appWorldInvoke.query.updateQuery()
        }
        
        attachedObjects: [
            Invocation{
                id: appWorldInvoke
                query {
                    invokeActionId: "bb.action.OPEN"
                    invokeTargetId: "sys.appworld"
                }
                onArmed: {
                    trigger(query.invokeActionId)
                }
            },
            QTimer {
                id: moveListViewTimer
                interval: 4000
                onTimeout: {
                    listView.iterator++
                    
                    if (listView.iterator == listView.dataModel.childCount([]))
                        listView.iterator = 0
                    
                    listView.scrollToItem([listView.iterator], ScrollAnimation.Smooth)
                    
                    if (listView.iterator == 0)
                        interval = 4500
                    else 
                        interval = 4000
                }
            },
            LayoutUpdateHandler {
                id: listViewLayoutUpdateHandler
            }
        ]  
        
        listItemComponents: [
            ListItemComponent {
                type: "item"
                Container {
                    id: itemContainer
                    preferredHeight: itemContainer.ListItem.view.maxHeight
                    minWidth: itemContainer.ListItem.view.itemWidth
                    maxWidth: minWidth
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill
                    Container {
                        layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                        rightPadding: itemContainer.ListItem.view.du(5)
                        leftPadding: rightPadding
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                        ImageView {
                            id: imageView
                            imageSource: "asset:///MyApps/" + ListItemData.appName + ".png"
                            preferredHeight: itemContainer.ListItem.view.maxHeight * 0.6667
                            preferredWidth: preferredHeight
                            scalingMethod: ScalingMethod.AspectFit
                            verticalAlignment: VerticalAlignment.Center
                        }
                        Container {
                            verticalAlignment: VerticalAlignment.Center
                            leftPadding: itemContainer.ListItem.view.du(5)
                            Label {
                                text: ListItemData.appName
                                horizontalAlignment: HorizontalAlignment.Center
                            }
                            Label {
                                text: ListItemData.price
                                horizontalAlignment: HorizontalAlignment.Center
                            }
                        }
                    }
                    Label {
                        text: qsTr(ListItemData.description)
                        multiline: true
                        horizontalAlignment: HorizontalAlignment.Center
                        minHeight: itemContainer.ListItem.view.maxHeight * 0.3333
                    }
                }
            }
        ]
    }
    CustomDivider {}    
}    
