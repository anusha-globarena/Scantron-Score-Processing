/*
 * Copyright (c) 2011 Akarsh Simha <akarshsimha@gmail.com>
 * License: GPL v2.05
 */

#include "versionlist.h"

bool VersionList::readKeysFromFile( QFile &VersionFile ) {
    QTextStream vs( &VersionFile );
    QTextStream cerr( stderr );
    int ln = 0;
    m_qcount = 0;
    bool retval = true;
    while( !vs.atEnd() ) {
        ln++;
        bool ok = false;
        QString record = vs.readLine();
        QStringList list = record.split( QString(",") );
        int versionid = list[0].toInt( &ok );
        if( !ok ) {
            cerr << "ERROR! On line " << ln << " of version data: " << list[0] << " is not a valid version number! Skipping this line!" << endl;
            retval = false;
            continue;
        }
        if( list.count() > MAX_QUESTIONS + 1 ) {
            cerr << "ERROR! On line " << ln << " of version data: A max of " << MAX_QUESTIONS << " are allowed! Skipping this line!" << endl;
            retval = false;
            continue;
        }
        if( m_qcount == 0 ) 
            m_qcount = list.count() - 1;
        if( list.count() - 1 != m_qcount ) {
            cerr << "ERROR! On line " << ln << " of version data: Wrong number of questions! Expected " << m_qcount << ", but have " << list.count() - 1 << " questions in version " << versionid << ". Skipping this line!" << endl;
            retval = false;
            continue;
        }
        if( m_VersionList.value( versionid ) ) {
            cerr << "ERROR! Duplicate version " << versionid << " on line " << ln << "! Ignoring the duplicate!" << endl;
            retval = false;
            continue;
        }
        m_VersionList.insert( versionid, new Version(versionid, list) );
    }
    return retval;
}
