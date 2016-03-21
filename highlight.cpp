#include "highlight.h"
#include <QPainter>
#include "constants.h"

Highlight::Highlight(QWidget *parent) : QWidget(parent)
{
    showThis = false;
}

void Highlight::paintEvent(QPaintEvent *)
{
    if (! showThis) return;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawRect(0, 0, TILE_SIZE, TILE_SIZE);
}

