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

#include "defines.h"
#include "studentlist.h"
#include "versionlist.h"

// TODO: Move score-calculating code to a new class and get rid of these includes
#include "version.h"
#include "student.h"

#include <iostream>

int main( int argc, char *argv[] ) {

    QApplication a( argc, argv );

    QTextStream cout( stdout );
    QTextStream cin( stdin );
    QTextStream cerr( stderr );

    QString filename( argv[1] );
    QFile ScoreFile( filename );

    /** CHECK IF WE'VE BEEN GIVEN ENOUGH ARGUMENTS **/
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

    /** OPEN THE SCORE FILE **/
    if( !ScoreFile.exists() ) {
        cerr<< "The score file " << filename << " does not exist!" << endl;
        return 1;
    }

    if( !ScoreFile.open( QIODevice::ReadOnly ) ) {
        cerr << "Failed to open " << filename << ". Please check permissions!" << endl;
        return 1;
    }

    /** POPULATE STUDENT RESPONSES **/
    StudentList studentlist;
    if ( !studentlist.readResponsesFromFile( ScoreFile ) ) {
        cerr << "POPULATING STUDENTS' LIST: SOMETHING WENT WRONG!!!!" << endl;
        cerr << "Please see the previous output for errors" << endl;
        cerr << "Continuing processing in 5 seconds" << endl;
        sleep(5);
    }
    ScoreFile.close();

    /** OPEN THE ANSWER KEY (VERSIONS) FILE **/
    filename = argv[2];
    QFile VersionFile( filename );

    if( !VersionFile.exists() ) {
        cerr << "The version file " << filename << " does not exist!" << endl;
        return 1;
    }

    if( !VersionFile.open( QIODevice::ReadOnly ) ) {
        cerr << "Failed to open " << filename << ". Please check permissions!" << endl;
        return 1;
    }

    /** OBTAIN ANSWER KEYS FOR ALL VERSIONS **/
    VersionList versionlist;
    if( !versionlist.readKeysFromFile( VersionFile ) ) {
        cerr << "READING ANSWER KEYS FROM VERSION FILE: ERRORS ENCOUNTERED!!!" << endl;
        cerr << "You have 5 seconds to hit Ctrl + C and abort if required" << endl;
        sleep(5);
    }


    /** PROCESS SCORES **/
    int student_number = 0;
    int qcount = versionlist.numQuestions();
    foreach( Student *currentStudent, *studentlist.list() ) {
        ++student_number;
        // Verify that the response contains only one filled bubble:
        Version *currentVersion = versionlist.list()[ currentStudent->version ];
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
                // TODO: Add support for weightages.
                currentStudent->score += currentVersion->Weightage[ qnum ];
                //                cerr << "DEBUG: Awarding points: score = " << currentStudent->score << endl;
            }
        }
    }
    cerr << "Information: Processed " << student_number << " students.";

    // Output scores
    foreach( Student *currentStudent, *studentlist.list() ) {
        cout << currentStudent->EID << "," << currentStudent->score << endl;
    }

}
