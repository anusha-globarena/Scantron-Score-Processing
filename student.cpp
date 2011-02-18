/*
 * Copyright (c) 2010 Akarsh Simha <akarshsimha@gmail.com>
 * License: GPL v2.05
 */

#include "student.h"
#include <qdebug.h>

Student::Student( const QString &_name, const QString &_EID, qint16 _version ) : name( _name ), EID( _EID ), version( _version ) {
    score = 0.0;
    for( int i = 0; i < MAX_QUESTIONS; ++i )
        responses[ i ] = 0;
}

bool Student::insertResponse( int question, QString response ) {
    qint16 binResponse = 0;
    for( int i = 1; i <= 9; ++i ) { // FIXME: What about 0 as a response?
        if( response.contains( QString::number( i ) ) )
            binResponse += ( 1 << i ); // NOTE: So 10 is the first response!
    }
    if( responses[ question ] != 0 ) {
        responses[ question ] = binResponse;
        return false;
    }
    responses[ question ] = binResponse;
    return true;
}

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

