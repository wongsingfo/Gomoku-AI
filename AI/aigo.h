#ifndef AIGO_H
#define AIGO_H

#include "ai_thread.h"

class AIGO : public AIThread
{
public:
    AIGO(QObject* parent = nullptr);
    virtual void run();

private:
};

#endif // AIGO_H
