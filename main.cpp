/*
 * Copyright (c) 2010 Akarsh Simha <akarshsimha@gmail.com>
 * License: GPL v2.05
 */

#include <QApplication>
#include <QFile>
#include <QIODevice>
#include <QString>
#include <QStringList>
#include <QList>
#include <QHash>
#include <QTextStream>

#include "student.h"

#include <iostream>

int main( int argc, char *argv[] ) {

    QHash<int, Version *> versions;
    QHash<QString, Student *> students;

    QApplication a( argc, argv );

    QTextStream cout( stdout );
    QTextStream cin( stdin );
    QTextStream cerr( stderr );
    QString filename( argv[1] );
    QFile f( filename );

    if( argc != 3 ) {
        // Display Usage and exit
        cerr << "USAGE: " << argv[0] << " <score file> <version file>" << endl;
        cerr << endl;
        cerr << "Score file is as output by the Quest Scantron Application" << endl;
        cerr << endl;
        cerr << "Version file contains one line describing the key to each version, as follows:" << endl;
        cerr << "version_number,key_1,key_2,key_3,...,key_n" << endl;
        cerr << "key_i is the answer key to the i^th problem. Multiple answers to be ORed are separated by \'|\'" << endl;
        cerr << "Eg: 345,1,1,3|4,5,6,2,3" << endl;
        cerr << endl;
        return 1;
    }

    if( !f.exists() ) {
        cerr<< "The file " << argv[1] << " does not exist!" << endl;
        return 1;
    }

    if( !f.open( QIODevice::ReadOnly ) ) {
        cerr << "Failed to open " << argv[1] << ". Please check permissions!" << endl;
        return 1;
    }

    /** TODO: Move this block to a separate class **/
    /** BEGIN BLOCK **/
    QTextStream ts( &f );
    int ln = 0;
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
        }
        if( !students.value( EID ) ) {
            students.insert( EID, new Student( "", EID, version ) ); // The name field is not yet used, so leaving it blank.
        }
        Student *current = students.value( EID );
        if( version != current->version ) {
            cerr << "Something's wrong. Version number " << version << "conflicts with previously provided " << current->version << ", on line " << ln << " for student " << EID << ". IGNORED." << endl;
        }
        int qnum = record.mid(32, 2).toInt( &ok );
        if( !ok ) {
            cerr << "FATAL: Something\'s wrong! The file has a question number that's not an integer: " << record.mid(32, 2) << ", on line " << ln << endl;
            return 1;
        }
        QString response = record.mid(35, 10);
        if( !current->insertResponse( qnum, response ) ) {
            cerr << "Duplicate response for question number " << qnum << " for student " << EID << " on line number " << ln << endl;
        }
    }
    /** END BLOCK **/
    f.close();

    
    // All students populated.

    // Now get versions

    filename = argv[2];
    QFile v( filename );

    /** TODO: Move this block to a separate class **/
    /** BEGIN BLOCK **/
    if( !v.exists() ) {
        cerr << "The file " << argv[2] << " does not exist!" << endl;
        return 1;
    }

    if( !v.open( QIODevice::ReadOnly ) ) {
        cerr << "Failed to open " << argv[2] << ". Please check permissions!" << endl;
        return 1;
    }

    QTextStream vs( &v );
    ln = 0;
    int qcount = 0;
    while( !vs.atEnd() ) {
        ln++;
        bool ok = false;
        QString record = vs.readLine();
        QStringList list = record.split( QString(",") );
        int versionid = list[0].toInt( &ok );
        if( !ok ) {
            cerr << "ERROR! On line " << ln << " of version data: " << list[0] << " is not a valid version number! Skipping this line!" << endl;
            continue;
        }
        if( list.count() > MAX_QUESTIONS + 1 ) {
            cerr << "ERROR! On line " << ln << " of version data: A max of " << MAX_QUESTIONS << " are allowed! Skipping this line!" << endl;
            continue;
        }
        if( qcount == 0 ) 
            qcount = list.count() - 1;
        if( list.count() - 1 != qcount ) {
            cerr << "ERROR! On line " << ln << " of version data: Wrong number of questions! Expected " << qcount << ", but have " << list.count() - 1 << " questions in version " << versionid << ". Skipping this line!" << endl;
            continue;
        }
        if( versions.value( versionid ) ) {
            cerr << "ERROR! Duplicate version " << versionid << " on line " << ln << "! Ignoring the duplicate!" << endl;
            continue;
        }
        versions.insert( versionid, new Version(versionid, list) );
    }
    /** END BLOCK **/
    
    ln = 0;
    foreach( Student *currentStudent, students ) {
        ++ln;
        // Verify that the response contains only one filled bubble:
        Version *currentVersion = versions[ currentStudent->version ];
        if( !currentVersion ) {
            cerr << "ERROR! Version " << currentStudent->version << " is undefined for " << currentStudent->EID << ". Skipping processing!" << endl;
            continue;
        }
        for(int qnum = 1; qnum <= qcount; ++qnum ) {
            qint16 response = currentStudent->responses[ qnum ];
            //            cerr << "DEBUG: Question " << qnum << endl;
            //            cerr << "DEBUG: Response " << response << endl;
            int sum = 0;
            for( int pwr = 0; pwr < MAX_QUESTIONS; pwr++ )
                sum = sum + ( ( ( 1 << pwr ) & response ) / ( 1 << pwr ) );
            
            if( sum > 1 ) {
                cerr << "ERROR! Student " << currentStudent->EID << " marked more than one answer for question " << qnum << ". Considering it wrong!" << endl;
                continue;
            }
            //            cerr << "DEBUG: Key " << currentVersion->Key[ qnum ] << endl;
            if( response & currentVersion->Key[ qnum ] ) {
                // Award points
                currentStudent->score += currentVersion->Weightage[ qnum ];
                //                cerr << "DEBUG: Awarding points: score = " << currentStudent->score << endl;
            }
        }
    }
    cerr << "Information: Processed " << ln << " students.";

    // Output scores
    foreach( Student *currentStudent, students ) {
        cout << currentStudent->EID << "," << currentStudent->score << endl;
    }

}
    
