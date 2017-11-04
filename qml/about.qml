// gpstuff -- alfonso martone

import QtQuick 2.0
import Sailfish.Silica 1.0

Page
{
    id: page
    onStatusChanged:
    {
        if (status === PageStatus.Active) {
            aboutpage.scrollToTop()
        }
    }

    ListModel
    {
        id: modelContacts

        ListElement
        {
            service: "Github sources"
            name: "ciofeca"
            url: "https://github.com/ciofeca/harbour-gpstuff"
        }

        ListElement
        {
            service: "Blog"
            name: "particolarmente urgentissimo"
            url: "https://particolarmente-urgentissimo.blogspot.com"
        }

        ListElement
        {
            service: "Twitter"
            name: "@ciofeca"
            url: "https://twitter.com/ciofeca"
        }
    }

    SilicaFlickable
    {
        id: aboutpage
        anchors.fill: page
        boundsBehavior: Flickable.DragAndOvershootBounds
        contentHeight: content.height

        Column
        {
            id: content
            width: parent.width
            spacing: Theme.paddingMedium
            PageHeader
            {
                title: "GPStuff "+Qt.application.version
            }

            Label {
                text: "<ul><li>shows/collects position data</li>
<li>also runs in background</li>
<li>can save in a textfile</li>
<li>temporarily stoppable</li>
<li>can hot-flag last position</li>
<li>highlights max speed/altitude</li>
<li>highlights total records</li>
</ul>"
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }

            anchors.leftMargin: Theme.paddingLarge
            anchors.rightMargin: Theme.paddingLarge

            Repeater
            {
                model: modelContacts
                delegate: BackgroundItem {
                    id: contact
                    width: page.width
                    onClicked: Qt.openUrlExternally(model.url)
                    Label {
                        id: contactLabel
                        anchors.left: parent.left; anchors.leftMargin: Theme.paddingLarge
                        anchors.right: parent.right; anchors.rightMargin: Theme.paddingLarge
                        anchors.verticalCenter: parent.verticalCenter
                        text: contact.pressed ? model.name : model.service
                        horizontalAlignment: contact.pressed ? Text.AlignLeft : Text.AlignRight
                        color: Theme.highlightColor
                    }
                }
            }

            Label
            {
                width: parent.width
                wrapMode: Text.WordWrap
                truncationMode: TruncationMode.Fade
                font.pixelSize: Theme.fontSizeSmall
                anchors.left: parent.left; anchors.leftMargin: Theme.paddingMedium
                anchors.right: parent.right; anchors.rightMargin: Theme.paddingMedium
                textFormat: Text.RichText;
                text: "<h2 align='center'>FAQ</h2>
<h3 align='center'><i>→ Why this software?</i></h3>
<ul><li>while commuting, I happen to stare at my phone looking at a bunch of wildly changing numbers;</li>
<li>and I love to bookmark the <i>exact</i> location of some interesting spot while on the move;</li>
<li>finally, at home, sometimes I like to fiddle with my list of positions/speeds/altitudes;</li>
<li>logging starts immediately because I don't want to miss positions if I'm already on the go.</li>
</ul>
<h3 align='center'><i>→ What does the main screen show?</i></h3>
<ul><li>latitude and longitude; current speed (km/hour); current clock and uptime since start; position in
degrees and altitude; satellites in view and available; maximum altitude and speed (don't exceed speed limits!);</li>
<li>below, two buttons: <i>bookmark</i> latest known position (will fill the clipboard with latest GPS position,
<i>flash</i> it on the screen for a second and mark a flag in the memory record),
and <i>graph</i> plot of the collected position data (green pixels, or white if <i>bookmarked);</i></li>
<li>note that correct 2-D positioning requires at least three satellites; 3-D requires 4; better if 5 or more.</li>
</ul>
<h3 align='center'><i>→ What does the 'cover' show?</i></h3>
<ul><li>either coordinates, or speed and total records, or speed and max speed;</li>
<li>you can switch mode flicking to the left; flicking to the right suspends/restarts position logging.</li>
</ul>
<h3 align='center'><i>→ Where is stored positioning data?</i></h3>
<ul><li>to an array in RAM (up to 99999 records): you will lose that data if you forget to save to a file
(autosaving is not privacy-compliant);</li>
<li>saved files go always in the <i>Documents</i> directory;</li>
<li>invalid values will always be zero.</li>
</ul>
<h3 align='center'><i>→ Will it drain my battery?</i></h3>
<ul><li>yes, depending on the <i>Settings →→ Location →→ Accuracy</i> features you chose to enable
(if you want useful data to save, at least <i>Device-only mode</i> should be set).</li>
</ul>
<h3 align='center'><i>→ Do I need to save GPX or TXT?</i></h3>
<ul><li>GPX files do not include speed, accuracy, and other fields.</li>
<li>GPX is only offered for compatibility with other software.</li>
<li>you can only save GPX from position graph page - this is intentional.</li>
</ul>
<h3 align='center'><i>→ Does it support maps services?</i></h3>
<ul><li>no.</li>
<li>This is a GPS position logger only. The graph map only plots in a flat plane the positioning data
coming from a spherical source;</li>
<li>when you <i>bookmark</i> a position, the clipboard buffer is filled with a link to OpenStreetMap;
then you can send an <i>I'm Here!</i> SMS/tweet pasting the contents of the clipboard.</li>
</ul>
<h3 align='center'><i>→ Why direction (current bearing, aka heading) is not shown/logged?</i></h3>
<ul><li>it may be unsupported by either the GPS driver or the Qt Location library;</li>
<li>note that <i>GPS heading</i> is not <i>compass sensor,</i> because the latter depends on phone orientation.</li>
</ul><br>"
            }
        }
    }
}

// ---
