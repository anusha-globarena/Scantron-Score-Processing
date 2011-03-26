/*
 * Copyright (c) 2011 Akarsh Simha <akarshsimha@gmail.com>
 * License: GPL v2.05
 */

#include <iostream>
#include <QFile>
#include <QTextStream>
#include <QList>

#include "studentlist.h"

bool StudentList::readResponsesFromFile( QFile &f ) {
    QTextStream cerr( stderr );
    QTextStream ts( &f );
    int ln = 0;
    bool retval = true;
    while( !ts.atEnd() ) {
        ln++;
        bool ok = false;
        QString record = ts.readLine();
        QString EID = record.mid(0, 11).trimmed().remove( ' ' ).toLower();
        QString versionstring = record.mid(11, 6);
        qint32 version = versionstring.toInt( &ok );
        if( !ok ) {
            cerr << "ERROR on line " << ln << ", EID = " << EID << ": Verison number " << record.mid(11, 6) << " is not an integer!" << endl;
            version = 0; // Fake version number is assigned
            retval = false;
        }
        if( !m_StudentList.value( EID ) ) {
            m_StudentList.insert( EID, new Student( "", EID, version ) ); // The name field is not yet used, so leaving it blank.
        }
        Student *current = m_StudentList.value( EID );
        if( version != current->version ) {
            cerr << "Something's wrong. Version number " << version << "conflicts with previously provided " << current->version << ", on line " << ln << " for student " << EID << ". IGNORED." << endl;
            retval = false;
        }
        int qnum = record.mid(32, 2).toInt( &ok );
        if( !ok ) {
            cerr << "FATAL: Something\'s wrong! The file has a question number that's not an integer: " << record.mid(32, 2) << ", on line " << ln << endl;
            return false;
        }
        QString response = record.mid(35, 10);
        if( !current->insertResponse( qnum, response ) ) {
            cerr << "Duplicate response for question number " << qnum << " for student " << EID << " on line number " << ln << endl;
            retval = false;
        }
    }
    return retval;
}
