TARGET       = harbour-gpstuff
CONFIG      += sailfishapp
QT          += quick positioning
SOURCES     += src/harbour-gpstuff.cpp
HEADERS     += src/harbour-gpstuff.h
OTHER_FILES += qml/harbour-gpstuff.qml     \
               qml/main.qml                \
               qml/cover.qml               \
               qml/about.qml               \
               qml/data.qml                \
               harbour-gpstuff.desktop     \
               rpm/harbour-gpstuff.changes \
               rpm/harbour-gpstuff.yaml
