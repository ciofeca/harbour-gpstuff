// gpstuff -- alfonso martone

#ifndef HARBOURGPSTUFF_H
#define HARBOURGPSTUFF_H

#define SUBVIEWS    4              // total variants in cover.qml
#define MAXRECORDS  99999          // archive size: more than 24 hours continuous
#define REFRESHING  4              // seconds before map-refresh
#define MINALT      -100           // min acceptable altitude (meters)
#define MAXALT      21111.111      // max acceptable altitude (meters)
#define MINSPD      0              // min acceptable speed (m/s)
#define MAXSPD      533.33333      // max acceptable speed (1920 km/h)
#define EXCLUDED_FAKE_POSITIONS  3 // Sailfish GPS apparently warms up with up to 3 fake positions

#define PROVIDER    "x"            // image provider used in data.qml
#define IMGLATLON   "image://" PROVIDER "/1"

#define SAVETEXT    ".txt"
#define SAVEGPX     ".gpx"         // not yet supported

#include <QDebug>
#include <QObject>
#include <QTimer>
#include <QDateTime>
#include <QElapsedTimer>
#include <QQuickImageProvider>

#include <QtPositioning/QGeoCoordinate>
#include <QtPositioning/QGeoPositionInfo>
#include <QtPositioning/QGeoPositionInfoSource>
#include <QtPositioning/QGeoSatelliteInfo>
#include <QtPositioning/QGeoSatelliteInfoSource>

#include <math.h>


enum ImageProvTypes { NOIMAGE, LATLON, IMAGEPROV };


class GPSdata
{
public:
    qint64 tim;                         // timestamp
    double lat, lon, spd, hac, vac;     // position, speed, accuracy
    int alt, head, sat;                 // altitude, heading, satellites
    bool flags;                         // bookmarked flag

    void setup(const double llat, const double llon, const qint64 ts) {
        lat=llat, lon=llon, tim=ts, spd=hac=vac=0.0, alt=sat=head=0, flags=0; }

    bool isValid() { return tim != 0; }

    void save(QTextStream& s, const char sep='\t', const char* endstr="\n")
    {
        QString q("%1%2%3%4%5%6%7%8%9%10%11%12%13%14%15%16%17%18%19%20");
        s << q.arg(tim).arg(sep).arg(lat,0,'f',7).arg(sep).arg(lon,0,'f',7).arg(sep).
             arg(spd,0,'f',1).arg(sep).arg(alt).arg(sep).arg(sat).arg(sep).
             arg(hac,0,'f',1).arg(sep).arg(vac,0,'f',1).arg(sep).
             arg(head).arg(sep).arg(flags).arg(endstr);
    }
};


class Position: public QObject, public QQuickImageProvider
{
    Q_OBJECT
    Q_PROPERTY(double  lat   READ lat   WRITE setLat   NOTIFY latChanged)
    Q_PROPERTY(double  lon   READ lon   WRITE setLon   NOTIFY lonChanged)
    Q_PROPERTY(int     alt   READ alt   WRITE setAlt   NOTIFY altChanged)
    Q_PROPERTY(int     altx  READ altx  WRITE setAltx  NOTIFY altxChanged)
    Q_PROPERTY(double  spd   READ spd   WRITE setSpd   NOTIFY spdChanged)
    Q_PROPERTY(double  spdx  READ spdx  WRITE setSpdx  NOTIFY spdxChanged)
    Q_PROPERTY(int     head  READ head  WRITE setHead  NOTIFY headChanged)
    Q_PROPERTY(int     sats  READ sats  WRITE setSats  NOTIFY satsChanged)
    Q_PROPERTY(int     satv  READ satv  WRITE setSatv  NOTIFY satvChanged)
    Q_PROPERTY(int     recs  READ recs  WRITE setRecs  NOTIFY recsChanged)
    Q_PROPERTY(int     run   READ run   WRITE setRun   NOTIFY runChanged)
    Q_PROPERTY(int     subv  READ subv  WRITE setSubv  NOTIFY subvChanged)
    Q_PROPERTY(int     flash READ flash WRITE setFlash NOTIFY flashChanged)
    Q_PROPERTY(QString sat   READ sat   WRITE setSat   NOTIFY satChanged)
    Q_PROPERTY(QString dir   READ dir   WRITE setDir   NOTIFY dirChanged)
    Q_PROPERTY(QString coord READ coord WRITE setCoord NOTIFY coordChanged)
    Q_PROPERTY(QString img   READ img   WRITE setImg   NOTIFY imgChanged)

public:
    explicit Position(int windowwidth, QObject* parent=NULL);
    ~Position() { delete[] gpsdata; gpsdata=NULL; }

    void setLat(const double &l)    { if(l != m_lat)   { m_lat = l;   emit latChanged();   } }
    void setLon(const double &l)    { if(l != m_lon)   { m_lon = l;   emit lonChanged();   } }
    void setAlt(const int& s)       { if(s != m_alt)   { m_alt = s;   emit altChanged();   } }
    void setAltx(const int& s)      { if(s > m_altx)   { m_altx = s;  emit altChanged();   } }
    void setSpd(const double& l)    { if(l != m_spd)   { m_spd = l;   emit spdChanged();   } }
    void setSpdx(const double& l)   { if(l > m_spdx)   { m_spdx = l;  emit spdxChanged();  } }
    void setHead(const int& s)      { if(s != m_head)  { m_head = s;  emit headChanged();  } }
    void setSats(const int& s)      { if(s != m_sats)  { m_sats = s;  emit satsChanged();  } }
    void setSatv(const int& s)      { if(s != m_satv)  { m_satv = s;  emit satvChanged();  } }
    void setRecs(const int& s)      { if(s != m_recs)  { m_recs = s;  emit recsChanged();  } }
    void setRun(const int& s)       { if(s != m_run)   { m_run = s;   emit runChanged();   } }
    void setSubv(const int& s)      { if(s != m_subv)  { m_subv = s;  emit subvChanged();  } }
    void setFlash(const int& s)     { if(s != m_flash) { m_flash = s; emit flashChanged(); } }
    void setSat(const QString& c)   { if(c != m_sat)   { m_sat = c;   emit satChanged();   } }
    void setDir(const QString& c)   { if(c != m_dir)   { m_dir = c;   emit dirChanged();   } }
    void setCoord(const QString& c) { if(c != m_coord) { m_coord = c; emit coordChanged(); } }
    void setImg(const QString& c)   { m_img = c; emit imgChanged();                          }

    double  lat()   const { return m_lat;   }  // latitude
    double  lon()   const { return m_lon;   }  // longitude
    int     alt()   const { return m_alt;   }  // altitude
    int     altx()  const { return m_altx;  }  // max altitude till now
    double  spd()   const { return m_spd;   }  // speed
    double  spdx()  const { return m_spdx;  }  // max speed till now
    int     head()  const { return m_head;  }  // heading
    int     sats()  const { return m_sats;  }  // satellites available
    int     satv()  const { return m_satv;  }  // satellites in view
    int     recs()  const { return m_recs;  }  // total records
    int     run()   const { return m_run;   }  // logging on/off flag
    int     subv()  const { return m_subv;  }  // current cover subview
    int     flash() const { return m_flash; }  // flashing position
    QString sat()   const { return m_sat;   }  // satellites text
    QString dir()   const { return m_dir;   }  // direction text
    QString coord() const { return m_coord; }  // coordinates text
    QString img()   const { return m_img;   }  // map image to provide

    Q_INVOKABLE QString elapsed();             // elapsed time
    Q_INVOKABLE QString osm();                 // map link on openstreetmap.org

    Q_INVOKABLE QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize);

signals:
    void latChanged();
    void lonChanged();
    void altChanged();
    void altxChanged();
    void spdChanged();
    void spdxChanged();
    void headChanged();
    void satsChanged();
    void satvChanged();
    void recsChanged();
    void runChanged();
    void subvChanged();
    void flashChanged();
    void satChanged();
    void dirChanged();
    void coordChanged();
    void imgChanged();

private:
    double m_lat, m_lon, m_spd, m_spdx, m_hac, m_vac;
    int m_alt, m_altx, m_head, m_sats, m_satv, m_recs, m_run, m_subv, m_flash;
    QString m_coord, m_sat, m_dir, m_img;
    QDateTime startuptime;
    QElapsedTimer uptime;
    QGeoPositionInfoSource* geosrc;
    QGeoSatelliteInfoSource* satsrc;
    GPSdata* gpsdata;
    GPSdata& last()        { return gpsdata[m_recs];   }  // sanitized outside
    GPSdata& prec()        { return gpsdata[m_recs-1]; }  // sanitized outsize
    qint64 started() const { return gpsdata[0].tim;    }  // sanitized outside
    int wg, hg;
    QImage pix;

    void drawLatLon(QImage& z);

public slots:
    void coverSubview();
    void startstop();
    void bookmark();
    void save(int flags=0);
    void resetFlash() { setFlash(false); }
    void positionUpdated(const QGeoPositionInfo &info);
    void satellitesInUseUpdated(const QList<QGeoSatelliteInfo>& info);
    void satellitesInViewUpdated(const QList<QGeoSatelliteInfo>& info);
};


// special characters used for GPS satellites ON/OFF strings:
#define GON  "\xe2\x97\x8f"
#define GOFF "\xe2\x97\x8b"


#endif // HARBOURGPSTUFF_H

// ---
