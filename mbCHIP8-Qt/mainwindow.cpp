#include <QtWidgets>
#include <QCoreApplication>
#include "mainwindow.h"
#include "assert.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <stdio.h>
#include <limits.h>
using namespace std;
#define SECOND 1
#define SAMPLING_HZ 44100
#define BUFFER_LENGTH (SECOND * SAMPLING_HZ)
#define SOUND_HZ 1040

ALCdevice *device;
ALCcontext *context;
ALshort dataqt[BUFFER_LENGTH * 2];
ALuint buffer;
ALuint source;
int i;

MainWindow::MainWindow()
{
    mbar = menuBar();
    createActions();
    createMenu();

    chip = new Chip8();
    display = new DisplayWidget(chip->m_ScreenData);
    setCentralWidget(display);

    stopped = true;

    win_width = 0;
    win_height = 0;

    initOpenAL();
    readSettings();
}

MainWindow::~MainWindow()
{
    stopOpenAL();
}

void MainWindow::initOpenAL()
{
    cout << "Loading OpenAL..." << endl;

    device = alcOpenDevice(NULL);
    context = alcCreateContext(device, NULL);
    alcMakeContextCurrent(context);
    alGenBuffers(1, &buffer);

    for (i = 0; i < BUFFER_LENGTH; i++)
    {
        dataqt[i * 2] = sin(2 * M_PI * SOUND_HZ * i / BUFFER_LENGTH) * SHRT_MAX;
        dataqt[i * 2 + 1] = -1 * sin(2 * M_PI * SOUND_HZ * i / BUFFER_LENGTH) * SHRT_MAX;
    }
    alBufferData(buffer, AL_FORMAT_STEREO16, dataqt, sizeof(dataqt), BUFFER_LENGTH * 2);
    alGenSources(1, &source);
    alSourcei(source, AL_BUFFER, buffer);
    alSourcei(source, AL_LOOPING, AL_TRUE);
}

void MainWindow::playQtAudio(unsigned char sound_timer)
{
    if (sound_timer > 0)
    {
        alSourcePlay(source);
    }
    else
    {
        alSourcePause(source);
    }
}

void MainWindow::stopOpenAL()
{
    cout << "Stopping OpenAL..." << endl;

    alSourceStop(source);
    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);
}

void MainWindow::openROM()
{
    filename = QFileDialog::getOpenFileName(this, tr("Please select a ROM..."), lastDir, tr("All files (*)"));
    lastDir = QFileInfo(filename).absolutePath();
    loadGame();
}

void MainWindow::loadGame()
{
    if (filename != "")
    {
        closeROM();
        chip->CPUReset();
        chip->LoadROM(filename.toLocal8Bit().data());
        stopped = false;
        emulation();
    }
}

void MainWindow::closeROM()
{
    stopped = true;
    display->clear();
    display->repaint();
}

void MainWindow::emulation()
{
    QElapsedTimer et;
    et.start();

    opcode_count = 0;
    int cycles_per_second = 10;

    while (!stopped)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        if (opcode_count < cycles_per_second)
        {
            chip->ExecuteNextOpcode(chip->m_Opcode);
            opcode_count++;
        }
        if (et.hasExpired(1000/60))
        {
            chip->DecreaseTimers();
            playQtAudio(chip->m_SoundTimer);
            et.restart();
            display->repaint();

            opcode_count = 0;
        }
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        case Qt::Key_0: chip->m_KeyData[0x0] = 1; break;
        case Qt::Key_1: chip->m_KeyData[0x1] = 1; break;
        case Qt::Key_2: chip->m_KeyData[0x2] = 1; break;
        case Qt::Key_3: chip->m_KeyData[0x3] = 1; break;
        case Qt::Key_4: chip->m_KeyData[0x4] = 1; break;
        case Qt::Key_5: chip->m_KeyData[0x5] = 1; break;
        case Qt::Key_6: chip->m_KeyData[0x6] = 1; break;
        case Qt::Key_7: chip->m_KeyData[0x7] = 1; break;
        case Qt::Key_8: chip->m_KeyData[0x8] = 1; break;
        case Qt::Key_9: chip->m_KeyData[0x9] = 1; break;
        case Qt::Key_A: chip->m_KeyData[0xA] = 1; break;
        case Qt::Key_B: chip->m_KeyData[0xB] = 1; break;
        case Qt::Key_C: chip->m_KeyData[0xC] = 1; break;
        case Qt::Key_D: chip->m_KeyData[0xD] = 1; break;
        case Qt::Key_E: chip->m_KeyData[0xE] = 1; break;
        case Qt::Key_F: chip->m_KeyData[0xF] = 1; break;
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        case Qt::Key_0: chip->m_KeyData[0x0] = 0; break;
        case Qt::Key_1: chip->m_KeyData[0x1] = 0; break;
        case Qt::Key_2: chip->m_KeyData[0x2] = 0; break;
        case Qt::Key_3: chip->m_KeyData[0x3] = 0; break;
        case Qt::Key_4: chip->m_KeyData[0x4] = 0; break;
        case Qt::Key_5: chip->m_KeyData[0x5] = 0; break;
        case Qt::Key_6: chip->m_KeyData[0x6] = 0; break;
        case Qt::Key_7: chip->m_KeyData[0x7] = 0; break;
        case Qt::Key_8: chip->m_KeyData[0x8] = 0; break;
        case Qt::Key_9: chip->m_KeyData[0x9] = 0; break;
        case Qt::Key_A: chip->m_KeyData[0xA] = 0; break;
        case Qt::Key_B: chip->m_KeyData[0xB] = 0; break;
        case Qt::Key_C: chip->m_KeyData[0xC] = 0; break;
        case Qt::Key_D: chip->m_KeyData[0xD] = 0; break;
        case Qt::Key_E: chip->m_KeyData[0xE] = 0; break;
        case Qt::Key_F: chip->m_KeyData[0xF] = 0; break;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    closeROM();
    writeSettings();
    event->accept();
}

void MainWindow::createActions()
{
    openROMAction = new QAction("Load ROM...", this);
    openROMAction->setShortcut(tr("Ctrl+O"));
    connect(openROMAction, &QAction::triggered, this, &MainWindow::openROM);

    closeROMAction = new QAction("Close ROM...", this);
    closeROMAction->setShortcut(tr("Ctrl+C"));
    connect(closeROMAction, &QAction::triggered, this, &MainWindow::closeROM);

    exitAction = new QAction("Exit", this);
    exitAction->setShortcut(tr("Alt+F4"));
    connect(exitAction, &QAction::triggered, this, &MainWindow::close);

    set1xAction = new QAction("64x32", this);
    set1xAction->setCheckable(true);
    connect(set1xAction, &QAction::triggered, this, &MainWindow::set1x);

    set2xAction = new QAction("128x64", this);
    set2xAction->setCheckable(true);
    connect(set2xAction, &QAction::triggered, this, &MainWindow::set2x);

    set4xAction = new QAction("256x128", this);
    set4xAction->setCheckable(true);
    connect(set4xAction, &QAction::triggered, this, &MainWindow::set4x);

    set8xAction = new QAction("512x256", this);
    set8xAction->setCheckable(true);
    connect(set8xAction, &QAction::triggered, this, &MainWindow::set8x);

    set12xAction = new QAction("768x384", this);
    set12xAction->setCheckable(true);
    connect(set12xAction, &QAction::triggered, this, &MainWindow::set12x);

    set16xAction = new QAction("1024x512", this);
    set16xAction->setCheckable(true);
    connect(set16xAction, &QAction::triggered, this, &MainWindow::set16x);

    resolutionGroup = new QActionGroup(this);
    resolutionGroup->addAction(set1xAction);
    resolutionGroup->addAction(set2xAction);
    resolutionGroup->addAction(set4xAction);
    resolutionGroup->addAction(set8xAction);
    resolutionGroup->addAction(set12xAction);
    resolutionGroup->addAction(set16xAction);

    bgColorDialogAction = new QAction("Background color...", this);
    connect(bgColorDialogAction, &QAction::triggered, this, &MainWindow::bgColorDialog);

    fgColorDialogAction = new QAction("Foreground color...", this);
    connect(fgColorDialogAction, &QAction::triggered, this, &MainWindow::fgColorDialog);

    pauseEmulation = new QAction("Pause", this);
    pauseEmulation->setCheckable(true);
    pauseEmulation->setShortcut(tr("P"));
    connect(pauseEmulation, &QAction::triggered, this, &MainWindow::pause);

    resetEmulation = new QAction("Reset...", this);
    resetEmulation->setShortcut(tr("R"));
    connect(resetEmulation, &QAction::triggered, this, &MainWindow::reset);

    aboutAction = new QAction("About", this);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::about);

    aboutQtAction = new QAction("About Qt...", this);
    connect(aboutQtAction, &QAction::triggered, this, &MainWindow::aboutQt);
}

void MainWindow::createMenu()
{
    fileMenu = mbar->addMenu("&File");
    fileMenu->addAction(openROMAction);
    fileMenu->addAction(closeROMAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    videoMenu = mbar->addMenu("&Video");
    resolutionMenu = videoMenu->addMenu("Window size...");
    resolutionMenu->addAction(set1xAction);
    resolutionMenu->addAction(set2xAction);
    resolutionMenu->addAction(set4xAction);
    resolutionMenu->addAction(set8xAction);
    resolutionMenu->addAction(set12xAction);
    resolutionMenu->addAction(set16xAction);
    videoMenu->addAction(bgColorDialogAction);
    videoMenu->addAction(fgColorDialogAction);

    emulationMenu = mbar->addMenu("&Emulation");
    emulationMenu->addAction(pauseEmulation);
    emulationMenu->addAction(resetEmulation);

    helpMenu = mbar->addMenu("&Help");
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);
}

void MainWindow::updateSize(int x, int y)
{
    display->repaint();
    if (x == 0 or y == 0)
    {
        x = win_width;
        y = win_height;
    }
    else
    {
        win_width = x;
        win_width = y;
    }
    setFixedSize(x, y + mbar->height());
}

void MainWindow::set1x()
{
    display->setResolution(1);
    updateSize(64, 32);
}

void MainWindow::set2x()
{
    display->setResolution(2);
    updateSize(128, 64);
}

void MainWindow::set4x()
{
    display->setResolution(4);
    updateSize(256, 128);
}

void MainWindow::set8x()
{
    display->setResolution(8);
    updateSize(512, 256);
}

void MainWindow::set12x()
{
    display->setResolution(12);
    updateSize(768, 384);
}

void MainWindow::set16x()
{
    display->setResolution(16);
    updateSize(1024, 512);
}

void MainWindow::writeSettings()
{
    QSettings settings("mbCHIP8", "mbCHIP8");
    settings.setValue("displayResolution", display->getResolution());
    settings.setValue("lastDir", lastDir);
    QVariant color = display->getBgColor();
    settings.setValue("bgColor", color);
    color = display->getFgColor();
    settings.setValue("fgColor", color);
}

void MainWindow::readSettings()
{
    QSettings settings("mbCHIP8", "mbCHIP8");
    int r = settings.value("displayResolution", 12).toInt();
    switch (r)
    {
        case 1: set1xAction->setCheckable(true); set1x(); break;
        case 2: set2xAction->setCheckable(true); set2x(); break;
        case 4: set4xAction->setCheckable(true); set4x(); break;
        case 8: set8xAction->setCheckable(true); set8x(); break;
        case 12: set12xAction->setCheckable(true); set12x(); break;
        case 16: set16xAction->setCheckable(true); set16x(); break;
    }
    lastDir = settings.value("lastDir", ".").toString();

    QVariant v = settings.value("bgColor", QColor(Qt::black));
    QColor color = v.value<QColor>();
    display->setBgColor(color);

    v = settings.value("fgColor", QColor(Qt::white));
    color = v.value<QColor>();
    display->setFgColor(color);
}

void MainWindow::bgColorDialog()
{
    display->setBgColor(QColorDialog::getColor(Qt::black));
    display->repaint();
}

void MainWindow::fgColorDialog()
{
    display->setFgColor(QColorDialog::getColor(Qt::white));
    display->repaint();
}

void MainWindow::reset()
{
    loadGame();
}

void MainWindow::pause()
{
    if (filename == "")
    {
        return;
    }
    if (pauseEmulation->isChecked())
    {
        stopped = true;
    }
    else
    {
        stopped = false;
        emulation();
    }
}

void MainWindow::about()
{
	QMessageBox::about(this, "About mbCHIP8-Qt", "<center><h3>mbCHIP8-Qt v0.1</h3></center>"+tr("<center><p>mbCHIP8 is a CHIP-8 emulator for Windows and Linux.</p><small><p>Copyright &copy; 2019 Buenia0.</a>.</p></small></center>"));
}

void MainWindow::aboutQt()
{
    QMessageBox::aboutQt(this, "About Qt");
}
