#ifndef TETRIS_H
#define TETRIS_H

#include <QtGui>
#include <QtCore>

class Tetris : public QMainWindow
{
    Q_OBJECT

public:
    Tetris(QWidget *parent = 0);
    ~Tetris();

    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);

    void Display(int now,int nowstatus,int x,int y,QPainter *painter);

    void MoveLeft();
    void MoveRight();
    void MoveDown();
    void MoveUp();
    void ChangeForm();
    void Over();
    void DeleteLine();
    void UpData();
    bool IsMoveOk(int,int);
    void GamesOver();

public slots:
    void Start();
    void Pause();
    void AboutMe();
    bool InitData();
    void timerEvent();
    void ReStart();

signals:
    void ScoreChanged(int score);
//    void LevelChanged(int level);
//    void LinesRemovedChanged(int numLines);

private:

    int curX;
    int curY;

    bool isStarted;
    bool isPaused;

    int now;
    int nowstatus;

    int next;
    int nextstatus;

    int score;
    int level;

    typedef struct display {
      int data[4][4];
    }DisplayData,*PDisplayData;

    DisplayData displaydata[7][4];

    int map[10][18][2];

    QAction *start;
    QAction *exit;
    QAction *about;

    QWidget *centralWidget;
    QGroupBox *nextbox;

    QGroupBox *control;
    QPushButton *button_start;
    QPushButton *button_stop;
    QGroupBox *group_sores;
    QLCDNumber *sores;

    QGroupBox *group_games;
    QMenuBar *menu;
    QMenu *menu_games;

    QTimer *t ;

};

#endif // TETRIS_H
