TARGET             = harbour-gpstuff
CONFIG            += sailfishapp
QT                += quick positioning
DEFINES           += APP_NAME=\"\\\"$${APPNAME}\\\"\" APP_AUTHOR=\"\\\"$${AUTHOR}\\\"\" APP_VERSION=\"\\\"$${VERSION}\\\"\"
SOURCES           += src/harbour-gpstuff.cpp
HEADERS           += src/harbour-gpstuff.h
SAILFISHAPP_ICONS += 86x86 108x108 128x128 256x256
DISTFILES         += icons/86x86/harbour-gpstuff.png \
                     icons/108x108/gpstuff-108.png   \
                     icons/128x128/gpstuff-128.png   \
                     icons/256x256/gpstuff-256.png
OTHER_FILES       += qml/harbour-gpstuff.qml         \
                     qml/main.qml                    \
                     qml/cover.qml                   \
                     qml/about.qml                   \
                     qml/data.qml                    \
                     harbour-gpstuff.desktop         \
                     rpm/harbour-gpstuff.changes     \
                     rpm/harbour-gpstuff.yaml
