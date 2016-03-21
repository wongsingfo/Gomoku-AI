#ifndef HIGHLIGHT_H
#define HIGHLIGHT_H

#include <QWidget>

class Highlight : public QWidget
{
    Q_OBJECT
public:
    explicit Highlight(QWidget *parent = 0);
    bool showThis;

signals:

public slots:

protected:
    void paintEvent(QPaintEvent *);
};

#endif // HIGHLIGHT_H
