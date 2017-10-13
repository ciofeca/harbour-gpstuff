harbour-gpstuff
===============

GPStuff -- a gps logger for Sailfish OS

RPM and installation instructions: https://openrepos.net/content/alfmar/gpstuff

Comments: https://talk.maemo.org/showthread.php?t=94420

Saved files in *Documents* directory are TXT files (all numeric fields, TAB-separated):
* timestamp as milliseconds since Unix epoch (aka classic Unix timestamp * 1000 + milliseconds)
* latitude
* longitude
* speed in km/hour, if available, else 0.0
* altitude in meters, if available, else 0
* satellites used for the current fix
* horizontal accuracy in meters, or 0.0
* vertical accuracy in meters, or 0.0
* GPS heading if available, or 0 (note: original Jolla phone and current SailfishX always emit 0)
* flags; current value: 0 for "not used", 1 for "bookmarked position"; future releases will treat this as a bitfield.
