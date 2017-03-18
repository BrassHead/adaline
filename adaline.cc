// adaline.cc  Simple neural net pattern matching
//
// Copyright (C) 1998-2013 Brian Bray
//

#include <fstream>
#include <iostream>
#include <cstdlib>

#include "bw/bwassert.h"
#include "bw/string.h"
#include "bw/process.h"
#include "bw/figure.h"
#include "bw/tools.h"
#include "bw/button.h"
#include "bw/scene.h"
#include "NetLayer.h"



using namespace bw;
using std::ifstream;
using std::cerr;
using std::endl;

const char *BWAppName = "hi";
const char *BWAppClass = "Hi";

const float weightScale = 100.;

Colour cscale(float fin)
{
    float flevel = fin*128.0 + 128.0;
    if (flevel>255.0)
        flevel = 255.0;
    if (flevel<0.0)
        flevel = 0.0;
    int level = (int)flevel;

    return Colour(level, level, level);
}


class NetControl : public Figure, Frame, Parent {
public:
    NetControl()
        : m_btnNext( "Next", "next();" ),
          m_btnTrain( "Train", "train();" ),
          m_btnExit( "Exit", "exit(0);" ),
          m_pNet( 0 ) {
        addChild( &m_btnNext, 20, FullHeight-50, 80, AskChild );
        addChild( &m_btnTrain, MidWidth, FullHeight-50, 80, AskChild );
        addChild( &m_btnExit, FullWidth-100, FullHeight-50, 80, AskChild );

        if (Process::getArgv()[1][0]=='p') {
            float theta = atof(++(Process::getArgv()[1]));
            m_pNet = new Perceptron( 63, 7, theta, 1.0 );
        } else {
            float alpha = atof(++(Process::getArgv()[1]));
            m_pNet = new Adaline( 63, 7, alpha );
        }
        step();
    }


    ~NetControl() {
        delete m_pNet;
    }


    void step() {
        // Reads next input set
        static int iFile = 0;
        static ifstream ifs;
        static char aszLine[40];
        static bool atEOF = true;
        static const String strTarg = "ABCDEJK";

        if (iFile==0) {
            iFile = 2;
        }

        if (atEOF) {
            ++iFile;
            if (iFile>=Process::getArgc())
                iFile = 3;
            m_strFileName = Process::getArgv()[iFile];
            ifs.open(m_strFileName);
            if (!ifs) {
                cerr << "Cannot open file " << m_strFileName << endl;
                exit(1);
            }
            atEOF = false;
        }

        ifs.getline(aszLine,40);
        if (ifs.eof()) {
            atEOF = true;
            ifs.close();
            step();		// Note: infinite loop if given empty file
        } else {
            // Reading target letter
            int iTarget = strTarg.indexOf(aszLine[0]);
            for (int i=0; i<7; i++) {
                if (iTarget==-1)
                    m_target[i] = 0.0;
                else if (iTarget==i)
                    m_target[i] = 1.0;
                else
                    m_target[i] = -1.0;
            }

            // Reading input pattern
            for (int i=0; i<9; i++) {
                ifs.getline(aszLine, 40);
                for (int j=0; j<7; j++) {
                    if (aszLine[j]==' ')
                        m_ins[i*7+j] = 0.0;
                    else if (aszLine[j]=='.' || aszLine[j]=='o')
                        m_ins[i*7+j] = -1.0;
                    else
                        m_ins[i*7+j] = 1.0;
                }
            }

            // Set Result
            m_pNet->eval(m_ins,m_result);
        }
    }

    void train() {
        // Reads training set
        static ifstream ifs;
        static char aszLine[40];
        static const String strTarg = "ABCDEJK";
        float target[7];
        float ins[63];

        String strFileName = Process::getArgv()[2];
        ifs.open(strFileName);
        if (!ifs) {
            cerr << "Cannot open file " << m_strFileName << endl;
            exit(1);
        }

        ifs.getline(aszLine,40);

        while (!ifs.eof()) {
            // Reading target letter
            int iTarget = strTarg.indexOf(aszLine[0]);
            for (int i=0; i<7; i++) {
                if (iTarget==-1)
                    target[i] = 0.0;
                else if (iTarget==i)
                    target[i] = 1.0;
                else
                    target[i] = -1.0;
            }

            // Reading input pattern
            for (int i=0; i<9; i++) {
                ifs.getline(aszLine, 40);
                for (int j=0; j<7; j++) {
                    if (aszLine[j]==' ')
                        ins[i*7+j] = 0.0;
                    else if (aszLine[j]=='.' || aszLine[j]=='o')
                        ins[i*7+j] = -1.0;
                    else
                        ins[i*7+j] = 1.0;
                }
            }

            // Train from result
            m_pNet->train(ins,target);

            // Read next target letter
            ifs.getline(aszLine,40);
        }

        ifs.close();
        m_pNet->eval(m_ins,m_result);
    }

    void draw(Canvas& c) {
        Press f(c);
        int w, h;
        getSize( w, h );
        f.drawText((w-f.getTextLength("Weights"))/2,20,"Weights");

        Brush brsh(c,Colour(128,128,128));
        Brush btmp(c);
        float wts[63];

        for (int i=0; i<7; i++) {
            brsh.fillRectangle( 10+60*i, 40, 53, 67 );
            m_pNet->getWeights(i,wts);
            for (int k=0; k<9; k++)
                for (int j=0; j<7; j++) {
                    btmp.setColour(cscale(wts[k*7+j]*weightScale));
                    btmp.fillRectangle( (10+60*i)+(3+7*j), 40+3+7*k, 6, 6 );
                }
        }

        int yio = 120;

        f.drawText( 50, yio+20, "Input" );
        brsh.fillRectangle( 50, yio+40, 53, 67 );
        for (int i=0; i<9; i++)
            for (int j=0; j<7; j++) {
                btmp.setColour(cscale(m_ins[i*7+j]));
                btmp.fillRectangle( 50+(3+7*j), yio+40+3+7*i, 6, 6 );
            }

        f.drawText( 150, yio+20, "Target" );
        brsh.fillRectangle( 150, yio+40, 53, 11 );
        for (int i=0; i<7; i++) {
            btmp.setColour(cscale(m_target[i]));
            btmp.fillRectangle( 150+(3+7*i), yio+40+3, 6, 6 );
        }

        f.drawText( 250, yio+20, "Output" );
        brsh.fillRectangle( 250, yio+40, 53, 11 );
        for (int i=0; i<7; i++) {
            btmp.setColour(cscale(m_result[i]));
            btmp.fillRectangle( 250+(3+7*i), yio+40+3, 6, 6 );
        }

        f.drawText( 350, yio+20, "Matches" );
        String str;
        for (int i=0; i<7; i++)
            if (m_result[i]>0.0)
                str.append("ABCDEJK"[i]);

        btmp.setColour(Colour(255,255,255));
        btmp.fillRectangle( 349, yio+30, 100, 30 );
        f.drawText( 350, yio+50, str );

    }

    bool onAction(const String& strCommand) {
        if (strCommand.startsWith("next")) {
            step();
            invalidate();
            return true;
        } else if (strCommand.startsWith("train")) {
            train();
            invalidate();
            return true;
        }
        return doAction(strCommand);
    }


    Button m_btnNext;
    Button m_btnTrain;
    Button m_btnExit;

    NetLayer*	m_pNet;
    float m_ins[63];
    float m_target[7];
    float m_result[7];

    String	m_strFileName;

};


class MyProcess : public Process {
public:
    int main(int argc, char*argv[]) {
        String strAction;
        Scene TheScene;

        if (getArgc()<4 ||
                (getArgv()[1][0]!='p' && getArgv()[1][0]!='a') ) {
            cerr << "Usage: adaline [ p<theta> | a<alpha> ] <train> <in1> <in2> ..." << endl;
            cerr << "Specifying a theta value creates a perceptron net" << endl;
            cerr << "Specifying an alpha value creates an adaline net" << endl;
            cerr << "See chapter 2 of 'Fundamentals of Neural Networks: "
                    "Architectures, Algorithm, Applications'" << endl;
            cerr << "by Laurene Fausett for details." << endl;
            return 1;
        }

        NetControl  mycontrol;
        TheScene.addFrame( &mycontrol );

        while ( !strAction.startsWith("exit(") )
            strAction = TheScene.nextAction();

        TheScene.removeFrame( &mycontrol );
        return atoi(strAction.substring(5));
    }
} myProcess;


