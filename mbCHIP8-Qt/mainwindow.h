#pragma once

#include <QMainWindow>
#include <QActionGroup>
#include "../libmbCHIP8/cpu.h"
#include "displaywidget.h"


class MainWindow : public QMainWindow
{
public:
    MainWindow();
    ~MainWindow();
    void readSettings();
    void writeSettings();
    void loadGame();
    void initOpenAL();
    void stopOpenAL();
    void playQtAudio(unsigned char sound_timer);
    QMenuBar *mbar;

private slots:
    void openROM();
    void closeROM();
    void updateSize(int, int);
    void set1x();
    void set2x();
    void set4x();
    void set8x();
    void set12x();
    void set16x();
    void bgColorDialog();
    void fgColorDialog();
    void reset();
    void pause();
    void about();
    void aboutQt();

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void closeEvent(QCloseEvent *event);

private:
    void createActions();
    void createMenu();
    void emulation();

    int win_width;
    int win_height;

    bool stopped;
    int opcode_count;

    QString filename;

    Chip8 *chip;
    DisplayWidget *display;

    QMenu *fileMenu;
    QMenu *videoMenu;
    QMenu *resolutionMenu;
    QActionGroup *resolutionGroup;
    QMenu *emulationMenu;
    QMenu *helpMenu;

    QAction *openROMAction;
    QAction *closeROMAction;
    QAction *exitAction;
    QAction *set1xAction;
    QAction *set2xAction;
    QAction *set4xAction;
    QAction *set8xAction;
    QAction *set12xAction;
    QAction *set16xAction;
    QAction *bgColorDialogAction;
    QAction *fgColorDialogAction;
    QAction *pauseEmulation;
    QAction *resetEmulation;
    QAction *aboutAction;
    QAction *aboutQtAction;

    QString lastDir;

};
