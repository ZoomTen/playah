#include "keyreader.h"

#include <stdio.h>
#include <unistd.h>
#include <termios.h>

static struct termios oldTerm, newTerm;

KeyReader::KeyReader()
{
    // disable key echoing
    tcgetattr(0, &oldTerm);
    newTerm = oldTerm;
    newTerm.c_lflag &= ~ICANON; // disable buffered io
    newTerm.c_lflag &= ~ECHO ; // disable echo
    tcsetattr(0, TCSANOW, &newTerm);
}

KeyReader::~KeyReader()
{
    // restore settings
    tcsetattr(0, TCSANOW, &oldTerm);
}

void KeyReader::run()
{
    while(true){
        char key = getchar();
        emit keyPressed(key);
    }
}
