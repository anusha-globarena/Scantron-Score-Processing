/*
 * Copyright (c) 2010 Akarsh Simha <akarshsimha@gmail.com>
 * License: GPL v2.05
 */

#include "version.h"

Version::Version( qint16 versionid, const QStringList &list ) {
    id = versionid;
    for( int qnum = 1; qnum < list.count(); ++qnum ) {
        qint16 binKey = 0;
        QStringList options = list[ qnum ].split( QString("|") );
        //        qDebug() << "Version " << versionid << "; Question " << qnum << "; Options are: " << options;
        foreach( const QString &correctOption, options ) {
            binKey = binKey | ( 1 << correctOption.toInt() ); // TODO: Check and report conversion failures
        }
        Key[ qnum ] = binKey;
        Weightage[ qnum ] = 1.0; // FIXME: Allow arbit weightages
    }
}

