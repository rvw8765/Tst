#include "mainwindow.h"
#include "diamonditem.h"
#include "simpleglwidget.h"


#include <QGraphicsScene>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QGroupBox>
#include <QStatusBar>
#include <QOffscreenSurface>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupViews();
    setupStatusBar();

    setWindowTitle(QStringLiteral("Diamond Test: Raster vs OpenGL"));
    resize(700, 450);
}

void MainWindow::setupViews()
{
    // --- Общая сцена ---
    auto *scene = new QGraphicsScene(this);
    scene->setSceneRect(-150, -150, 300, 300);
    scene->setBackgroundBrush(Qt::lightGray);

    // Синий ромб (с антиальясингом) — слева
    auto *diamond = new DiamondItem();
    diamond->setPos(-50, 0);
    scene->addItem(diamond);

    // Красный «пиксельный» круг (без антиальясинга) — справа
    auto *pixel = new PixelItem();
    pixel->setPos(50, 0);
    scene->addItem(pixel);

    // --- View #1: растровый ---
    auto *rasterView = new QGraphicsView(scene);
    rasterView->setRenderHint(QPainter::Antialiasing, true);
    rasterView->setMinimumSize(300, 300);

    // --- View #2: OpenGL + MSAA ---


    auto *glView = new QGraphicsView(scene);
    auto *glWidget = new SimpleGLWidget(glView);
    glView->setViewport(glWidget);
    glView->setRenderHint(QPainter::Antialiasing, true);
    glView->setMinimumSize(300, 300);

    // --- Оформление ---
    auto *rasterBox = new QGroupBox(QStringLiteral("QGraphicsView (raster)"));
    auto *rasterLay = new QVBoxLayout(rasterBox);
    rasterLay->addWidget(rasterView);

    auto *glBox = new QGroupBox(QStringLiteral("QGraphicsView (OpenGL, MSAA)"));
    auto *glLay = new QVBoxLayout(glBox);
    glLay->addWidget(glView);

    auto *central = new QWidget(this);
    auto *layout = new QHBoxLayout(central);
    layout->addWidget(rasterBox);
    layout->addWidget(glBox);
    setCentralWidget(central);
}

void MainWindow::setupStatusBar()
{
    QOpenGLContext ctx;
    ctx.setFormat(QSurfaceFormat::defaultFormat());

    QString text = QStringLiteral("OpenGL: контекст не создан");

    if (ctx.create()) {
        QOffscreenSurface surface;
        surface.setFormat(ctx.format());
        surface.create();

        if (surface.isValid() && ctx.makeCurrent(&surface)) {
            QOpenGLFunctions *f = ctx.functions();

            const char *vendor   = reinterpret_cast<const char *>(f->glGetString(GL_VENDOR));
            const char *renderer = reinterpret_cast<const char *>(f->glGetString(GL_RENDERER));
            const char *version  = reinterpret_cast<const char *>(f->glGetString(GL_VERSION));
            const char *glsl     = reinterpret_cast<const char *>(f->glGetString(GL_SHADING_LANGUAGE_VERSION));

            // фактическое число сэмплов у созданного контекста
            const int samples = ctx.format().samples();

            text = QStringLiteral("OpenGL %1 | Vendor: %2 | Renderer: %3 | GLSL: %4 | MSAA: %5x")
                       .arg(QString::fromLatin1(version  ? version  : "?"))
                       .arg(QString::fromLatin1(vendor   ? vendor   : "?"))
                       .arg(QString::fromLatin1(renderer ? renderer : "?"))
                       .arg(QString::fromLatin1(glsl     ? glsl     : "?"))
                       .arg(samples);

            ctx.doneCurrent();
        }
    }

    statusBar()->showMessage(text);
}
