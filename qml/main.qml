// gpstuff -- alfonso martone

import QtQuick 2.0
import Sailfish.Silica 1.0

Page
{
    id: page

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
                text: qsTr("Save textfile into Documents")
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
            width: page.width
            spacing: Theme.paddingLarge

            PageHeader
             {
                title: GPS.run ? qsTr("GPS logging: ")+GPS.recs : qsTr("GPS stopped: ")+GPS.recs
            }

            Label
            {
                x: Theme.paddingLarge
                text: GPS.flash ? "<b>"+GPS.lat+"</b>" : GPS.lat;
            }

            Label
            {
                x: Theme.paddingLarge
                text: GPS.flash ? "<b>"+GPS.lon+"</b>" : GPS.lon;
            }

            Label
            {
                width: parent.width-2*Theme.paddingLarge
                x: Theme.paddingLarge
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Text.AlignRight
                text: GPS.spd + " km/h  "+GPS.dir
            }

            Label
            {
                x: Theme.paddingLarge;
                id: uptext
            }

            Label
            {
                x: Theme.paddingLarge
                anchors.topMargin: Theme.paddingLarge
                text: GPS.coord
            }

            Label
            {
                x: Theme.paddingLarge
                text: GPS.sat
                anchors.topMargin: Theme.paddingLarge
            }

            Label
            {
                x: Theme.paddingLarge
                width: page.width-Theme.paddingLarge*2
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Text.AlignHCenter
                text: "<i>max -- "+GPS.altx+" m -- "+GPS.spdx+" km/h</i>"
            }

            Button
            {
                signal bookmark()
                x: Theme.paddingLarge
                width: page.width-Theme.paddingLarge*2
                anchors.topMargin: Theme.paddingLarge*2
                text: GPS.run ? qsTr("Bookmark/copy position") : qsTr("Bookmark/copy last known pos")
                onClicked: GPS.bookmark()
            }

            Button
            {
                x: Theme.paddingLarge
                width: page.width-Theme.paddingLarge*2
                anchors.topMargin: Theme.paddingLarge
                text: qsTr("Position graph")
                enabled: GPS.recs>3
                onClicked: pageStack.push(Qt.resolvedUrl("data.qml"))
            }
        }
    }
}

// ---
