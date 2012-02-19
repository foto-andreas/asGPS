#include "iso3166.h"

#include <QTextStream>
#include <QFile>
#include <QDebug>

Iso3166::Iso3166()
{
}

void Iso3166::clear() {

    map23.clear();
    map32.clear();
    map2n.clear();

}

bool Iso3166::load(QString nameOrResource) {
    try {
        QFile file(nameOrResource);
        file.open( QIODevice::ReadOnly);
        QTextStream ts(&file);
        while (!ts.atEnd()) {
            QString line = ts.readLine();
            if (line.isEmpty() || line.startsWith("#")) {
                continue;
            }
            QString cc2 = line.mid(0,2);
            QString cc3 = line.mid(3,3);
            line.remove(0,11);
            map23.insert(cc2,cc3);
            map32.insert(cc3,cc2);
            map2n.insert(cc2,line);

        }
        file.close();
    } catch (int) {
        return false;
    }
    return true;
}

const QString Iso3166::from2to3(QString twoLetterCode) {
    QMap<QString,QString>::iterator i = map23.find(twoLetterCode);
    if (i==map23.end()) return "";
    return i.value();
}

const QString Iso3166::from3to2(QString threeLetterCode) {
    QMap<QString,QString>::iterator i = map32.find(threeLetterCode);
    if (i==map32.end()) return "";
    return i.value();
}

const QString Iso3166::name(QString twoLetterCode) {
    QMap<QString,QString>::iterator i = map2n.find(twoLetterCode);
    if (i==map2n.end()) return "";
    return i.value();
}

