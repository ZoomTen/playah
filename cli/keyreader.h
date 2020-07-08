#ifndef KEYREADER_H
#define KEYREADER_H

#include <QThread>

class KeyReader : public QThread
{
    Q_OBJECT
public:
    KeyReader();
    ~KeyReader();
    void run();
signals:
    void keyPressed(char key);
};

#endif // KEYREADER_H
