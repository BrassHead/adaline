// Creates Chapter 2 data with some randomization
//

#include <fstream.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char*argv[])
{
    ifstream ifs;
    char aszLine[40];
    char ins[63];
    char outs[64];	// One extra
    char target;
    int indx;
    char val;

    if (argc!=2) {
        cerr << "Usage: credata <input>" << endl;
        return 1;
    }

    ifs.open(argv[1]);
    if (!ifs) {
        cerr << "Cannot open file " << argv[1] << endl;
        exit(1);
    }

    ifs.getline(aszLine,40);

    while (!ifs.eof()) {
        // Reading target letter
        target = aszLine[0];

        // Reading input pattern
        for (int i=0; i<9; i++) {
            ifs.getline(aszLine, 40);
            for (int j=0; j<7; j++) {
                ins[i*7+j] = aszLine[j];
            }
        }

        // Randomize and output
        for (int i=0; i<1000; i++) {
            memcpy(outs,ins,63);
            for (int j=0; j<5; j++)
                outs[rand()&63] = "  .x"[rand()&3];

            // Write it
            cout << target << endl;
            for (int i=0; i<9; i++) {
                for (int j=0; j<7; j++) {
                    cout << outs[i*7+j];
                }
                cout << endl;
            }

        }

        // Read next target
        ifs.getline(aszLine,40);
    }
    ifs.close();

    return 0;
}
