#include <QMessageBox>
#include <QTimer>
#include <QMouseEvent>
#include <QDebug>
#include <QRectF>
#include <QPainter>
#include <qmath.h>
#include "gamewidget.h"
#include "QTime"



GameWidget::GameWidget(QWidget *parent) :
    QWidget(parent),
    timer(new QTimer(this)),
    timerColor(new QTimer(this)),
    generations(-1),
    ca1(),
    universeSize(50),
    universeMode(0),
    cellMode(0),
    randomMode(0),
    lifeTime(50)
{
    timer->setInterval(300);
    timerColor->setInterval(50);
    m_masterColor = "#000";
    ca1.ResetWorldSize(universeSize, universeSize);
    connect(timer, SIGNAL(timeout()), this, SLOT(newGeneration()));
    connect(timerColor, SIGNAL(timeout()), this, SLOT(newGenerationColor()));

}

GameWidget::~GameWidget()
{

}

void GameWidget::startGame(const int &number)  // Start Game
{
    generations = number;
    timer->start();
}

void GameWidget::stopGame()  //Stop Game
{
    timer->stop();
    timerColor->stop();
}

void GameWidget::clear() // Clear game field
{
    for(int k = 1; k <= universeSize; k++) {
        for(int j = 1; j <= universeSize; j++) {
            ca1.SetAlive(j,k,0);
        }
    }
    gameEnds(true);
    ca1.ResetWorldSize(universeSize,universeSize);
    randomMode = 0;
    update();

}

int GameWidget::cellNumber()  // Number of the cells in one row
{
    return universeSize;
}

void GameWidget::setCellNumber(const int &s) // Set number of the cells in one row
{
    universeSize = s;
    ca1.ResetWorldSize(s,s);
    update();
}

int GameWidget::lifeInterval() //cell's lifetime - number of step when cell is on the universe
{
    return lifeTime;
}

void GameWidget::setLifeInterval(const int &l) // set lifetime for cell
{
    lifeTime = l;
}

void GameWidget::setMode(const int &m) //Set evolution mode
{
        universeMode = m;
}

void GameWidget::setCellMode(const int &m) //Set cell mode
{
    cellMode = m;
}


QString GameWidget::dump() // Dump of current universe
{
    char temp;
    QString master = "";
    for(int k = 1; k <= universeSize; k++) {
        for(int j = 1; j <= universeSize; j++) {
            if(ca1.IsAlive(j,k) == 1) {
                temp = '*';
            } else {
                temp = 'o';
            }
            master.append(temp);
        }
        master.append("\n");
    }
    return master;
}

void GameWidget::setDump(const QString &data) // Set current universe from it's dump
{
    int current = 0;
    for(int k = 1; k <= universeSize; k++) {
        for(int j = 1; j <= universeSize; j++) {
           if(data[current] == '*') ca1.SetAlive(j,k,1);
            current++;
        }
        current++;
    }
    update();
}

int GameWidget::interval() // Interval between generations
{
    return timer->interval();
}

void GameWidget::setInterval(int msec) // Set interval between generations
{
    timer->setInterval(msec);
}

void GameWidget::newGeneration()  // Start the evolution of universe and update the game field
{
    if(generations < 0)
        generations++;

    ca1.WorldEvolutionLife();

    update();

    if(ca1.IsNotChanged()) {
        QMessageBox::information(this,
                                 tr("Game lost sense"),
                                 tr("The End. Now game finished because all the next generations will be the same."),
                                 QMessageBox::Ok);
        stopGame();
        gameEnds(true);
        return;
    }

    generations--;
    if(generations == 0) {
        stopGame();
        gameEnds(true);
        QMessageBox::information(this,
                                 tr("Game finished."),
                                 tr("Iterations finished."),
                                 QMessageBox::Ok,
                                 QMessageBox::Cancel);
    }
}

void GameWidget::newGenerationColor() // Start the evolution of universe and update the game field for "Cyclic cellular automata" mode
{
    if(generations < 0)
        generations++;

    update();

    generations--;
    if(generations == 0) {
        stopGame();
        gameEnds(true);
        QMessageBox::information(this,
                                 tr("Game finished."),
                                 tr("Iterations finished."),
                                 QMessageBox::Ok,
                                 QMessageBox::Cancel);
    }
}

void GameWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    paintGrid(p);
    paintUniverse(p);
}

void GameWidget::mousePressEvent(QMouseEvent *e)
{
    emit environmentChanged(true);
    double cellWidth = (double)width()/universeSize;
    double cellHeight = (double)height()/universeSize;
    int k = floor(e->y()/cellHeight)+1;
    int j = floor(e->x()/cellWidth)+1;

    int mode[9] = {1, 3, 6, 4, 2, 8, 9, 10, 11};

    if(ca1.IsAlive(j,k) != 0){
        ca1.SetAlive(j,k,0);
        ca1.SetLife(j,k,0);
    }
    else {
        ca1.SetAlive(j,k,mode[cellMode]);
        if(mode[cellMode] == 9 || mode[cellMode] == 10)
            ca1.SetLife(j,k,lifeTime);
    }

    update();
}

void GameWidget::mouseMoveEvent(QMouseEvent *e)
{
    double cellWidth = (double)width()/universeSize;
    double cellHeight = (double)height()/universeSize;
    int k = floor(e->y()/cellHeight)+1;
    int j = floor(e->x()/cellWidth)+1;

    int mode[9] = {1, 3, 6, 4, 2, 8, 9, 10, 11};

    if(ca1.IsAlive(j,k) == 0){
        ca1.SetAlive(j,k,mode[cellMode]);
        if(mode[cellMode] == 9 || mode[cellMode] == 10)
            ca1.SetLife(j,k,lifeTime);
        update();
    }
}

void GameWidget::paintGrid(QPainter &p)
{
    QRect borders(0, 0, width()-1, height()-1); // borders of the universe
    QColor gridColor = m_masterColor; // color of the grid
    gridColor.setAlpha(10); // must be lighter than main color
    p.setPen(gridColor);
    double cellWidth = (double)width()/universeSize; // width of the widget / number of cells at one row
    for(double k = cellWidth; k <= width(); k += cellWidth)
        p.drawLine(k, 0, k, height());
    double cellHeight = (double)height()/universeSize; // height of the widget / number of cells at one row
    for(double k = cellHeight; k <= height(); k += cellHeight)
        p.drawLine(0, k, width(), k);
    p.drawRect(borders);
}

void GameWidget::paintUniverse(QPainter &p)
{
    double cellWidth = (double)width()/universeSize;
    double cellHeight = (double)height()/universeSize;
    for(int k=1; k <= universeSize; k++) {
        for(int j=1; j <= universeSize; j++) {
            if(ca1.IsAlive(j, k)!=0){
                qreal left = (qreal)(cellWidth*j-cellWidth); // margin from left
                qreal top  = (qreal)(cellHeight*k-cellHeight); // margin from top
                QRectF r(left, top, (qreal)cellWidth, (qreal)cellHeight);
                if(randomMode && universeMode != 7)
                    p.fillRect(r, SetColor(ca1.GetColor(j,k))); //fill cell with brush from random mode
                else{
                    if(ca1.IsAlive(j, k) == 1 || universeMode == 7)
                        p.fillRect(r, QBrush(m_masterColor)); // fill cell with brush of main color
                    else
                        p.fillRect(r, SetColor(ca1.IsAlive(j, k))); //fill cell with brush of cell type
                }
            }
        }
    }
}

QColor GameWidget::masterColor()
{
    return m_masterColor;
}

QColor GameWidget::SetColor(const int &color)
{
    QColor cellColor[12]= {Qt::red, Qt::darkRed, Qt::green, Qt::darkGreen, Qt::blue, Qt::darkBlue, Qt::cyan, Qt::darkCyan, Qt::magenta, Qt::darkMagenta, Qt::yellow, Qt::darkYellow};

    if(color >= 0 && color < 12)
        cellColor[color];

    return cellColor[color];
}

void GameWidget::setMasterColor(const QColor &color)
{
    m_masterColor = color;
    update();
}

