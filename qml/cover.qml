// gpstuff -- alfonso martone

import QtQuick 2.0
import Sailfish.Silica 1.0

CoverBackground
{
    id: cover

    Column
    {
        id: column
        width: cover.width
        spacing: Theme.paddingMedium

        PageHeader
        {
            title: GPS.run ? qsTr("logging") : qsTr("stopped")
        }

        Label
        {
            x: Theme.paddingLarge
            text: (GPS.subv == 0 ? GPS.lat : (GPS.run ? GPS.spd+" km/h" : "---") )
        }

        Label
        {
            x: Theme.paddingLarge
            width: parent.width-2*Theme.paddingLarge
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Text.AlignRight
            text: (GPS.subv == 0 ? GPS.lon : (GPS.subv == 1 ? GPS.recs :
                                                              (GPS.subv == 2 ? "<i>(max "+GPS.spdx+")</i>" :
                                                                               ("<i>"+GPS.head+"° "+GPS.dir+"</i>"))))
        }

        CoverActionList
        {
            id: coverAction

            CoverAction
            {
                signal startstop()
                iconSource: GPS.run ? "image://theme/icon-cover-pause" : "image://theme/icon-cover-next"
                onTriggered: GPS.startstop()
            }

            CoverAction
            {
                signal subview()
                iconSource: "image://theme/icon-cover-subview"
                onTriggered: GPS.coverSubview()
            }
        }
    }
}

// ---
