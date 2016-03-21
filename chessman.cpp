#include "chessman.h"
#include <QPainter>
#include <QDebug>

Chessman::Chessman(QWidget *parent) : QWidget(parent)
{

}

void Chessman::setType(ChessType x)
{
    type = x;
    update();
}

void Chessman::paintEvent(QPaintEvent *)
{
    if (type == EMPTY) return;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    if (type == WHITE)
        painter.setBrush(Qt::white);
    else if (type == BLACK)
        painter.setBrush(Qt::gray);
    else Q_ASSERT(false);
    painter.drawEllipse(TILE_MARGIN, TILE_MARGIN,
                        TILE_SIZE - TILE_MARGIN * 2, TILE_SIZE - TILE_MARGIN * 2);
}

