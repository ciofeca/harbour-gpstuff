// gpstuff -- alfonso martone

import QtQuick 2.0
import Sailfish.Silica 1.0

Page
{
    id: data

    SilicaFlickable
    {
        anchors.fill: parent

        PullDownMenu
        {
            MenuItem
            {
                text: qsTr("About GPStuff")
                onClicked: pageStack.push(Qt.resolvedUrl("about.qml"))
            }

            MenuItem
            {
                signal save()
                text: qsTr("Save TXT file into Documents")
                enabled: GPS.recs>0
                onClicked: GPS.save(0)
            }

            MenuItem
            {
                signal startstop()
                text: GPS.run ? qsTr("Stop GPS") : qsTr("Restart GPS")
                onClicked: GPS.startstop()
            }
        }

        contentHeight: column.height

        Timer
        {
            interval: 1000;
            running: true;
            repeat: true;
            onTriggered: uptext.text = "<i>"+GPS.elapsed()+"</i>"
        }

        Column
        {
            id: column
            width: data.width
            spacing: Theme.paddingLarge

            PageHeader
            {
                title: GPS.run ? qsTr("logging: ")+GPS.recs : qsTr("stopped: ")+GPS.recs
            }

            Label
            {
                x: Theme.paddingLarge
                text: GPS.sat
            }

            Label
            {
                x: Theme.paddingLarge;
                id: uptext
                anchors.bottomMargin: Theme.paddingLarge*2
            }

            Image
            {
                cache: false
                source: GPS.img
            }

            Button
            {
                signal bookmark()
                x: Theme.paddingLarge
                width: data.width-Theme.paddingLarge*2
                anchors.topMargin: Theme.paddingLarge*2
                text: GPS.run ? qsTr("Bookmark/copy position") : qsTr("Bookmark/copy last known pos")
                onClicked: GPS.bookmark()
            }

            Button
            {
                text: "OpenStreetMap"
                x: Theme.paddingLarge
                width: data.width-Theme.paddingLarge*2
                anchors.topMargin: Theme.paddingLarge*2
                enabled: GPS.recs>3
                onClicked: Qt.openUrlExternally(GPS.osm())
            }
        }
    }
}

// ---
