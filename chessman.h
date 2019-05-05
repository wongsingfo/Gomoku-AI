#ifndef CHESSMAN_H
#define CHESSMAN_H

#include <QWidget>
#include "constants.h"

class Chessman : public QWidget
{
    Q_OBJECT
public:
    explicit Chessman(QWidget *parent = nullptr);
    void setType(ChessType);

protected:
    void paintEvent(QPaintEvent *);
    ChessType type;

signals:

public slots:
};

#endif // CHESSMAN_H
