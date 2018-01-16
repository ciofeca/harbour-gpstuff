/* gpstuff -- alfonso martone
 *
 * a two evenings project and probably my biggest code mess of the last 20 years
 *
 */

#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include <QGuiApplication>
#include <QClipboard>
#include <QQuickView>
#include <QQmlContext>
#include <QScreen>
#include <QStandardPaths>

#include <sailfishapp.h>

#include "harbour-gpstuff.h"


int main(int argc, char* argv[])  // typical SailfishApp create/view/exec initialization
{
    QString name(APP_NAME), vers(APP_VERSION), org(APP_AUTHOR),
            prov(PROVIDER), path("qml/" APP_NAME ".qml");
    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    app->setApplicationName(name);
    app->setApplicationVersion(vers);
    app->setOrganizationName(org);

    // the constructor expects a portrait-aligned display width:
    Position gps(app->screens().first()->availableSize().width());

    QScopedPointer<QQuickView> view(SailfishApp::createView());
    view->setTitle(name);
    view->engine()->addImageProvider(prov, &gps);
    view->rootContext()->setContextProperty("GPS", &gps);
    view->setSource(SailfishApp::pathTo(path));
    view->show();
    view->showFullScreen();

    // cannot cleanly "return app->exec()" because of 
    // a weird "double free or corruption (out): 0xffa310d8" error
    // happening in QImage (requestImage) at least up to SailfishOS 2.1.3
    //
    exit(app->exec());
    return 0;
}


// main class setup
//
Position::Position(int displaywidth, QObject* parent):
    QObject(parent), QQuickImageProvider(QQuickImageProvider::Image),
    m_lat(444.555), m_lon(444.666), m_spd(0.0), m_spdx(0.0), m_hac(0.0), m_vac(0.0),
    m_lon0(444.111), m_lat0(444.222), m_lonx(-444.333), m_latx(-444.444),
    m_alt(0), m_altx(0), m_head(0), m_sats(0), m_satv(0), m_recs(0), m_run(0),
    m_subv(SUBVIEWS), m_flash(0), m_coord(" "), m_sat(" "), m_dir(" "), m_img(" "), gpsdata(NULL)
{
    startuptime = QDateTime::currentDateTime();
    m_utc = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
    uptime.start();

    gpsdata = new GPSdata[MAXRECORDS];
    setImg(IMGLATLON);

    wg = displaywidth;
    hg = (wg/4)*3;           // a 4:3 area; works best on vertical displays
    coverSubview();

    // hook the location provider
    //
    geosrc = QGeoPositionInfoSource::createDefaultSource(this);
    if(geosrc)
    {
        geosrc->setPreferredPositioningMethods(QGeoPositionInfoSource::AllPositioningMethods);
        satsrc = QGeoSatelliteInfoSource::createDefaultSource(this);
        if(satsrc)
        {
            connect(satsrc, SIGNAL(satellitesInUseUpdated(const QList<QGeoSatelliteInfo>&)),
                    this,     SLOT(satellitesInUseUpdated(const QList<QGeoSatelliteInfo>&)));
            connect(satsrc, SIGNAL(satellitesInViewUpdated(const QList<QGeoSatelliteInfo>&)),
                    this,     SLOT(satellitesInViewUpdated(const QList<QGeoSatelliteInfo>&)));

            satsrc->startUpdates();
            satsrc->setUpdateInterval(1000); // require updates every 1000 msec
        }

        connect(geosrc, SIGNAL(positionUpdated(QGeoPositionInfo)),
                this,     SLOT(positionUpdated(QGeoPositionInfo)));

        geosrc->startUpdates();
        geosrc->setUpdateInterval(1000);    // msec

        setRun(1);
    }
    else
    {
        qDebug() << "TREMEND ERROR: no GPS DefaultSource available (FlightMode active?)";
        QCoreApplication::exit(123);
    }
}


QString Position::elapsed()  // current date/time and elapsed time
{
    qint64 e = uptime.elapsed();
    int min = e/60000;
    int sec = (e/1000)%60;
    return QString("%1 -- up: %2:%3").
            arg(QDateTime::currentDateTime().toString("h.mm")).
            arg(min).arg(sec,2,10,QChar('0'));
}


void Position::startstop()   // toggle GPS updates
{
    if(geosrc)
    {
        if(m_run)
        {
            geosrc->stopUpdates();
            if(satsrc) satsrc->stopUpdates();
            setRun(0);
        }
        else
        {
            geosrc->startUpdates();
            if(satsrc) satsrc->startUpdates();
            setRun(1);
        }
    }
}


void Position::coverSubview()   // switch to next Cover subview
{
    int nxt = m_subv;
    if(++nxt >= SUBVIEWS) nxt = 0;
    setSubv(nxt);
}


// when a new GPS position comes in: validate and update
//
void Position::positionUpdated(const QGeoPositionInfo &info)
{
    double r, lat, lon;

    // we log anything claiming to have valid coordinates
    //
    if(info.coordinate().isValid())
    {
        // fetch latitude/longitude but truncate to six decimal digits
        lat = floor(info.coordinate().latitude() *1000000.0)/1000000.0;
        lon = floor(info.coordinate().longitude()*1000000.0)/1000000.0;
        last().setup(lat, lon, info.timestamp().isValid() ?
                         info.timestamp().toMSecsSinceEpoch() : 0);

        // update the bounding box only if enough satellites in use
        if(m_sats >= 3)
        {
            if(m_lon0 > lon) m_lon0 = lon;
            if(m_lat0 > lat) m_lat0 = lat;
            if(m_lonx < lon) m_lonx = lon;
            if(m_latx < lat) m_latx = lat;
        }

        // update the current record
        //
        setLat(lat);
        setLon(lon);

        r = info.attribute(QGeoPositionInfo::HorizontalAccuracy);
        if(std::isnormal(r) && r>0.0 && r<9999.555) m_hac=r; else m_hac=0.0;
        r = info.attribute(QGeoPositionInfo::VerticalAccuracy);
        if(std::isnormal(r) && r>0.0 && r<9999.555) m_vac=r; else m_vac=0.0;
        last().hac = m_hac, last().vac = m_vac;

        r = info.coordinate().altitude();
        if(std::isnormal(r) && r>=MINALT && r<MAXALT)
        {
            setAlt((int)r);
            setAltx(m_alt);
            last().alt = m_alt;
        }

        r = info.attribute(QGeoPositionInfo::GroundSpeed);
        if(std::isnormal(r) && r>=MINSPD && r<MAXSPD)
        {
            setSpd(floor(r*36.0)/10.0); // convert m/s to km/hr with single decimal digit
            setSpdx(m_spd);
            last().spd = m_spd;
        }

        last().sat = m_satv;

        r = info.attribute(QGeoPositionInfo::MagneticVariation);
        if(std::isnormal(r))
        {
            // note: Jolla phone and XperiaX GPS libraries do not yet support heading (magnetic variation)
            setHead((int)r);
            int u = (int)((r-22.5)/45.0);
            switch(u)
            {
            default: setDir(QString("(N)")); break;
            case 1:  setDir(QString("(NE)")); break;
            case 2:  setDir(QString("(E)")); break;
            case 3:  setDir(QString("(SE)")); break;
            case 4:  setDir(QString("(S)")); break;
            case 5:  setDir(QString("(SW)")); break;
            case 6:  setDir(QString("(W)")); break;
            case 7:  setDir(QString("(NW)")); break;
            }
            last().head = m_head;
        }

        setCoord(info.coordinate().toString());
        if(last().isValid())
        {
            if(m_recs < MAXRECORDS)
            {
                setRecs(m_recs + 1);
                if(0 == (m_recs % REFRESHING))
                {
                    setImg("");         // force update
                    setImg(IMGLATLON);
                }
            }
        }
    }
}


void Position::satellitesInUseUpdated(const QList<QGeoSatelliteInfo>& info)
{
    setSats(info.size());
    QString q;
    switch(m_sats)
    {
    case 0:  q = GOFF GOFF GOFF GOFF GOFF; break;  // no satellites in use
    case 1:  q = GON  GOFF GOFF GOFF GOFF; break;  // 1 satellite in use
    case 2:  q = GON  GON  GOFF GOFF GOFF; break;
    case 3:  q = GON  GON  GON  GOFF GOFF; break;
    case 4:  q = GON  GON  GON  GON  GOFF; break;  // 4 satellites in use
    default: q = GON  GON  GON  GON  GON;  break;  // plenty of satellites
    }
    setSat(QString(q.append(" (%1/%2)").arg(m_sats).arg(m_satv)));
}


void Position::satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &info)
{
    setSatv(info.size());
    QString q;
    switch(m_sats)
    {
    case 0:  q = GOFF GOFF GOFF GOFF GOFF; break;  // no satellites in use
    case 1:  q = GON  GOFF GOFF GOFF GOFF; break;  // 1 satellite in use
    case 2:  q = GON  GON  GOFF GOFF GOFF; break;
    case 3:  q = GON  GON  GON  GOFF GOFF; break;
    case 4:  q = GON  GON  GON  GON  GOFF; break;  // 4 satellites in use
    default: q = GON  GON  GON  GON  GON;  break;  // plenty of satellites
    }
    setSat(QString(q.append(" (%1/%2)").arg(m_sats).arg(m_satv)));
}


QString Position::osm()  // create an OpenStreetMap map URI for last position
{
    QString s("https://www.osm.org/?mlat=%1&mlon=%2#map=15/%3/%4");
    return s.arg(prec().lat,0,'f',7).arg(prec().lon,0,'f',7).
             arg(prec().lat,0,'f',5).arg(prec().lon,0,'f',5);
}


void Position::bookmark()
{
    if(m_recs==0) return;
    prec().flags = true;   // flag record as "bookmarked"

    setFlash(true);        // "flash" the screen text indicato rfor a while
    QTimer::singleShot(800, this, SLOT(resetFlash()));  // schedule the "unflash"

    // insert into clipboard a tweetable string including OSM map and a few extras
    QDateTime t;
    t.setMSecsSinceEpoch(prec().tim);
    QString sms(MYPOSTR);  // something like "I'm currently here (MyPosition): "
    sms += osm();
    sms += " -- accuracy: %1m -- altitude: %2m -- speed: %3 km/h -- %4";
    QClipboard& c = *QGuiApplication::clipboard();
    c.setText(sms.arg((int)(prec().hac)).arg(prec().alt).
                  arg(prec().spd,0,'f',1).arg(t.toString("hh:mm:ss")));
}


// save in Documents location the current positions
//
void Position::save(int gpxmode)
{
    QString fname(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    fname += "/gps-%1";
    fname = fname.arg(startuptime.toString("yyyyMMdd-hhmmss"));

    if(gpxmode) fname += SAVEGPX; else fname += SAVETEXT;

    QFile fd(fname);
    if(fd.open(QFile::WriteOnly | QFile::Truncate))
    {
        qDebug() << "saving" << m_recs << "records to" << fname;
        int n;
        QTextStream fp(&fd);

        if(gpxmode)
        {
            QString hdr("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<gpx version=\"1.0\">"
                        "<name>GPStuff %1</name>\n");
            fp << hdr.arg(m_utc);

            for(n=0; n<m_recs; n++)
            {
                if(gpsdata[n].flags)    // bookmarked positions are saved as "waypoints"
                {
                    QString q("<wpt lat=\"%1\" lon=\"%2\"><ele>%3</ele><name>+</name></wpt>\n");
                    fp << q.arg(gpsdata[n].lat,0,'f',7).arg(gpsdata[n].lon,0,'f',7).arg(gpsdata[n].alt);
                }
            }

            fp << "<trk><name>track</name><number>1</number><trkseg>\n";

            for(n=0; n<m_recs; n++) gpsdata[n].savegpx(fp);

            fp << "</trkseg></trk>\n</gpx>\n";
        }
        else
        {
          for(int n=0; n<m_recs; n++) gpsdata[n].save(fp);
        }
    }
    else
    {
        qDebug() << "TREMEND ERROR creating file:" << fname;
        exit(1);
        // being unable to write in the Documents directory
        // means that the filesystem is in a quite bad state...
    }
}


// draw a non-proportional 2D projection of latitude/longitude values
//
void Position::drawLatLon(QImage& z)
{
    int x,y,n;

    // return if we don't have yet reasonable values
    if(noboxyet()) return;

    // watch out for tiny x/y ranges
    double x0=m_lon0, y0=m_lat0, xr=m_lonx-m_lon0, yr=m_latx-m_lat0;
    if(xr<0.0001) xr=0.0001, x0-=0.00005;
    if(yr<0.0001) yr=0.0001, y0-=0.00005;

    // plot positions will be in green, bookmarked ones in white:
    QRgb grn=qRgb(0,255,0), wht=qRgb(255,255,255);

    bool nodraw = true;
    for(n=0; n<m_recs; n++)
    {
        // don't draw anything until a real GPS position is available
        if(gpsdata[n].sat >= 3) nodraw = false;
        if(nodraw) continue;

        // fetch coordinates and sanitize
        x = (int)((gpsdata[n].lon-x0)/xr*wg);
        y = hg-(int)((gpsdata[n].lat-y0)/yr*hg);
        if(x<0) x=0;
        if(x>=wg) x=wg-1;
        if(y<0) y=0;
        if(y>=hg) y=hg-1;

        if(gpsdata[n].flags & 1)        // bookmarked?
        {
            z.setPixel(x,             y,   wht);
            if(x>0)    z.setPixel(x-1,y,   wht);
            if(x<wg-1) z.setPixel(x+1,y,   wht);
            if(y>0)    z.setPixel(x,  y-1, wht);
            if(y<hg-1) z.setPixel(x,  y+1, wht);
        }
        else
        {
            z.setPixel(x, y, grn);
        }
    }
}


// build updated image on demand
//
QImage Position::requestImage(const QString& id, QSize* size, const QSize& requestedSize)
{
    pix = QImage(wg, hg, QImage::Format_ARGB32_Premultiplied);
    QRgb blk = qRgb(0,0,0), red = qRgb(255,0,0);
    bool ok = false;
    int n = id.toInt(&ok);
    if(!ok || n<1 || n>=IMAGEPROV) n = 0;

    switch(n)
    {
    case LATLON:  pix.fill(blk); drawLatLon(pix); break;
    default:      pix.fill(red);                  break;
    }

    if(size) *size = pix.size();
    if(requestedSize.width() > 0 && requestedSize.height() > 0)
        return pix.scaled(requestedSize.width(), requestedSize.height(), Qt::IgnoreAspectRatio);
    return pix;
}

// --- ⎋
