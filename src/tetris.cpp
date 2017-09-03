#include "tetris.h"

const QColor colorTable[9] = {
        QColor(0xDAAA00),QColor(0xCC6666),QColor(0x66CC66),
        QColor(0x6666CC),QColor(0xCCCC66),QColor(0xCC66CC),
        QColor(0xAADDFF),QColor(0xFFFFFF),QColor(0xFFFFFF),
    };

Tetris::Tetris(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Tetris");
    setMinimumSize(350, 370);
    setMaximumSize(350, 370);

    start = new QAction("ReStart",this);
    connect(start,SIGNAL(triggered()),this,SLOT(ReStart()) );

    exit = new QAction("Exit",this);
    connect(exit,SIGNAL(triggered()),qApp,SLOT(quit()));

    centralWidget = new QWidget(this);
    nextbox = new QGroupBox("Next",centralWidget);
    nextbox->setGeometry(220, 10, 120, 120);

    control = new QGroupBox("Contrl",centralWidget);
    control->setGeometry(220, 250, 120, 90);

    button_start = new QPushButton("Start",control);
    button_start->setFocusPolicy(Qt::NoFocus);
    button_start->setGeometry(20, 20, 80, 25);
    connect(button_start,SIGNAL(clicked()),this,SLOT(Start()));

    button_stop = new QPushButton("Pause",control);
    button_stop->setFocusPolicy(Qt::NoFocus);
    button_stop->setGeometry(20, 50, 80, 25);
    connect(button_stop,SIGNAL(clicked()),this,SLOT(Pause()));

    group_sores = new QGroupBox("Sores",centralWidget);
    group_sores->setGeometry(220, 150, 120, 80);

    sores = new QLCDNumber(group_sores);
    sores->setSegmentStyle(QLCDNumber::Filled);
    sores->setGeometry(10, 20, 100, 50);
    connect(this,SIGNAL(ScoreChanged(int)),sores,SLOT(display(int)));

    group_games = new QGroupBox("Games",centralWidget);
    group_games->setGeometry(10, 10, 180, 330);

    setCentralWidget(centralWidget);

    menu = new QMenuBar(this);
    menu->setGeometry(0, 0, 350, 20);

    menu_games = new QMenu("Menu",menu);

    about = new QAction("About",this);
    connect(about,SIGNAL(triggered()),this,SLOT(AboutMe()));

    setMenuBar(menu);

    menu->addAction(menu_games->menuAction());

    menu_games->addAction(start);
    menu_games->addAction(about);
    menu_games->addAction(exit);

    if(!InitData())
    {
        QMessageBox::information(this, tr("Error Tetris"),
             tr("The datafile <b>in.txt</b> loaded failed!"));
    }

    t = new QTimer(this);
    t->setInterval(1000);
    connect(t, SIGNAL(timeout()), this, SLOT(timerEvent()));
}

bool Tetris::InitData()
{
     QFile file("in.txt");
     if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
     {
         return false;
     }
     QTextStream in(&file);

     for(int k=0; k<7; k++)
         for(int m=0; m<4; ++m)
             for(int i=0; i<4; ++i)
                 for(int j=0 ; j<4; ++j)
                     in>>displaydata[k][m].data[i][j] ;

    next = rand()%7;
    nextstatus = rand()%4;

    for(int k=0; k<10; ++k)
    {
        for(int j=0; j<18; ++j)
        {
            map[k][j][0]=0;
            map[k][j][1]=7;
        }
    }

    score = 0;
    level = 1;

    emit ScoreChanged(score);

    UpData();

    return true;
}

void Tetris::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QColor color;
    painter.setRenderHint(QPainter::Antialiasing);

    painter.translate(250,65);

    Display(next,nextstatus,0,0,&painter);

    painter.translate(-222,-5);
    for(int i=0; i<10 ; ++i)
    {
        for(int j=0; j<18; ++j)
        {
            painter.setPen(QPen(colorTable[map[i][j][1]], 16, Qt::SolidLine, Qt::SquareCap));
            painter.drawLine(16*i,16*j,16*i,16*j);
        }
    }

    Display(now,nowstatus,curX,curY,&painter);
}

void Tetris::Display(int n,int s,int x,int y,QPainter *painter)
{
    for(int i=0; i<4; i+=1)
    {
        for(int j=0; j<4; j+=1)
        {
            if(1 == displaydata[n][s].data[i][j])
            {
                painter->setPen(QPen(colorTable[n], 16, Qt::SolidLine, Qt::SquareCap));
                painter->drawLine(16*(i+x),16*(j+y),16*(i+x),16*(j+y));
            }
        }
    }
}

void Tetris::MoveLeft()
{
    if(IsMoveOk(curX -1 ,curY))
    {
        curX = curX -1;
        update();
    }
}

void Tetris::MoveRight()
{
    if(IsMoveOk(curX +1 ,curY))
    {
        curX = curX +1;
        update();
    }
}

void Tetris::MoveDown()
{
    if(IsMoveOk(curX ,curY + 1))
    {
        curY = curY +1;
        update();
    }
    else
        Over();
}

void Tetris::MoveUp()
{
    while(IsMoveOk(curX ,curY - 1))
        curY = curY - 1;
  //  curY = curY - 1;
}

void Tetris::ChangeForm()
{
    int i = nowstatus;
    nowstatus = ( nowstatus + 1)%4;
    if(IsMoveOk(curX,curY))
        update();
    else
        nowstatus = i;
}

void Tetris::Over()
{
    for(int i=0; i<4; ++i)
    {
        for(int j=0; j<4; ++j)
        {
            if(1 == displaydata[now][nowstatus].data[i][j])
            {
               map[curX+i][curY+j][0] = 1;
               map[curX+i][curY+j][1] = now;
            }
        }
    }
    score += rand()%50;
    emit ScoreChanged(score);

    DeleteLine();
    UpData();
}

void Tetris::DeleteLine()
{
    int j,k,i,s=0;
    for(j=0; j<18; ++j)
    {
        for(i=0; i<10; ++i)
        {
            if(map[i][j][0]==0)
                break;
        }
        if(i==10)
        {
            for(k=j;j>0;--j)
            {
                for(i=0; i<10; ++i)
                {
                    map[i][j][0]=map[i][j-1][0];
                    map[i][j][1]=map[i][j-1][1];
                }
            }j=k-1;
            s++;
        }
    }
    if(s>0)
    {
        switch(s)
        {
            case 1:score += 50 ;break;
            case 2:score += 100 ;break;
            case 3:score += 200 ;break;
            case 4:score += 400 ;break;
        }
        emit ScoreChanged(score);
    }
}

void Tetris::UpData()
{
    now = next;
    nowstatus = nextstatus;

    next = rand()%7;
    nextstatus = rand()%4;
    curX = 3;
    curY = 0;

    MoveUp();

    if(!(IsMoveOk(curX ,curY)))
        GamesOver();
}

void Tetris::GamesOver()
{
    Pause();

    int ret = QMessageBox::warning(this, tr("Games Over !"),
                                tr("The games is over .\n"
                                   "Do you want to have a new games ?"),
                                QMessageBox::Ok | QMessageBox::Cancel | QMessageBox::Close,
                                QMessageBox::Ok);
    switch(ret)
    {
        case QMessageBox::Ok:ReStart();break;
        case QMessageBox::Cancel:;break;
        case QMessageBox::Close:qApp->quit();break;
    }

}

bool Tetris::IsMoveOk(int x,int y)
{
    for(int i=0; i<4; ++i)
    {
        for(int j=0; j<4; ++j)
        {
            if(1 == displaydata[now][nowstatus].data[i][j])
            {
               if((x+i)<0 || (x+i)>9 || (y+j)>17 || (y+j)<0 || map[x+i][y+j][0])
                    return false;
            }
        }
    }
    return true;
}

void Tetris::keyPressEvent(QKeyEvent *event)
{
    if ( isPaused )
    {
        QWidget::keyPressEvent(event);
        return;
    }

    switch (event->key())
    {
    case Qt::Key_Left:MoveLeft();break;
    case Qt::Key_Right:MoveRight();break;
    case Qt::Key_Down:MoveDown();break;
    case Qt::Key_Up:ChangeForm();break;
    case Qt::Key_Space:
        {
        } break;
    default:
        QWidget::keyPressEvent(event);
    }
}

void Tetris::timerEvent()
{
    MoveDown();
}

void Tetris::AboutMe()
{
    QMessageBox::information(this, tr("About Tetris"),
             tr("The <b>Tetris</b> design by linimbus@126.com ! "));
}

void Tetris::Pause()
{
    t->stop();
    isPaused = 1;
}

void Tetris::Start()
{
    t->start();
    isPaused = 0;
}

void Tetris::ReStart()
{
    InitData();
    update();
    Start();
}

Tetris::~Tetris()
{

}
