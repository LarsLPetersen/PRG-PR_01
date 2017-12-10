#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QColor>
#include <QWidget>
#include <QObject>
#include "CAbase.h"


class GameWidget : public QWidget {

    Q_OBJECT

public:
    explicit GameWidget(QWidget *parent = 0);
    ~GameWidget();
    CAbase getCA();

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    //void keyPressEvent(QKeyEvent *e);

signals:
    void universeModified(bool);
    void gameStarted(bool);
    void gameStopped(bool);
    void gameEnds(bool);


public slots:
    void startGame(const int &number = -1); // start
    void stopGame(); // finish
    void clearGame(); // clear

    int getUniverseSize(); // number of the cells in one row
    void setUniverseSize(const int &s); // set number of the cells in one row

    int getUniverseMode();
    void setUniverseMode(const int &m); //set evolution mode

    int getCellMode();
    void setCellMode(const int &m); //set cell mode

    int getInterval(); // interval between generations
    void setInterval(int msec); // set interval between generations

    //int getLifeInterval(); // cell's lifetime - number of step when cell is on the universe
    //void setLifeInterval(const int &l); // set lifetime for cell

    QColor getMasterColor(); // color of the cells
    void setMasterColor(const QColor &color); // set color of the cells
    QColor setColor(const int &color); //set color of the cells from their number

    QString dumpGame(); // dump of current universe
    void reconstructGame(const QString &data); // set current universe from it's dump

    //
    // snake
    //
    void calcDirectionSnake (int dS);

    void setDirectionSnake(int past, int future);

    void setPositionSnakeHead(int x, int y);

    void setPositionFood(int x, int y);

    void setSnakeLength(int l);

    void setSnakeAction(int a);


private slots:
    void paintGrid(QPainter &p);
    void paintUniverse(QPainter &p);
    void newGeneration();
    void newGenerationColor();

private:
    QColor masterColor;
    QTimer *timer;
    QTimer *timerColor;
    int generations;
    CAbase ca1;
    int universeSize;
    int universeMode;
    int cellMode;
    //int randomMode;
    //int lifeTime;
};


#endif // GAMEWIDGET_H
