#include "simpleglwidget.h"
#include <QDebug>
#include <QTimer>

SimpleGLWidget::SimpleGLWidget(QWidget *parent)
{
    QSurfaceFormat fmt = this->format();
    fmt.setRenderableType(QSurfaceFormat::OpenGL);
    //fmt.setVersion(3, 3);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    fmt.setRenderableType(QSurfaceFormat::OpenGL);
    fmt.setAlphaBufferSize(0);
    fmt.setDepthBufferSize(24);
    fmt.setSamples(32);
    this->setFormat(fmt);

    QTimer *t = new QTimer(this);
    t->setInterval(3000);
    t->start();
    QObject::connect(t, &QTimer::timeout, this, &SimpleGLWidget::checkGLState);
}

void SimpleGLWidget::initializeGL()
{
    // 1) Инициализируем указатели на функции OpenGL для текущего контекста.
    //    После этого можно вызывать glClear(), glGetString(), glGetIntegerv() и т.д.
    initializeOpenGLFunctions();

    // 2) Узнаём, что реально выдал драйвер.
    const GLubyte *vendorPtr   = glGetString(GL_VENDOR);
    const GLubyte *rendererPtr = glGetString(GL_RENDERER);
    const GLubyte *versionPtr  = glGetString(GL_VERSION);

    const QString vendor   = vendorPtr   ? QString::fromLatin1(reinterpret_cast<const char *>(vendorPtr))   : QStringLiteral("?");
    const QString renderer = rendererPtr ? QString::fromLatin1(reinterpret_cast<const char *>(rendererPtr)) : QStringLiteral("?");
    const QString version  = versionPtr  ? QString::fromLatin1(reinterpret_cast<const char *>(versionPtr))  : QStringLiteral("?");

    // 3) Проверяем MSAA на уровне текущего FBO.
    //    QOpenGLWidget использует внутренний FBO; именно его сэмплы нам интересны.
    GLint samples = 0;
    glGetIntegerv(GL_SAMPLES, &samples);
    GLint maxSamples = 0;
    glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);

    qDebug() << Q_FUNC_INFO << samples << maxSamples;

    // 4) Если драйвер дал сэмплы — включаем мультисэмплинг явно.
    //    Формально он должен быть включён по умолчанию, но некоторые
    //    драйверы/платформы (например, ANGLE) требуют явного вызова.
    if (samples > 0) {
        glEnable(GL_MULTISAMPLE);
    }

    qDebug() << "GLViewportWidget::initializeGL"
             << "vendor ="   << vendor
             << "renderer =" << renderer
             << "version ="  << version
             << "samples ="  << samples;

    // 5) Сообщаем наружу — MainWindow обновит статус-бар.
    //emit glInitialized(samples, vendor, renderer, version);

    // 6) Базовая настройка состояния — то, что обычно делают в initializeGL.
    glClearColor(0.85f, 0.85f, 0.85f, 1.0f); // совпадает с lightGray фона сцены
    glDisable(GL_DEPTH_TEST);                 // 2D-сцена, глубина не нужна
}

void SimpleGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    GLint samples = 0;
    glGetIntegerv(GL_SAMPLES, &samples);
    // Если выводится 0, значит MSAA не активирован; если >0, значит включен
    qDebug() << "Actual FBO samples:" << samples;

    // Явно включаем мультисэмплинг (некоторые драйверы требуют явного вызова перед каждым рисованием)
    if (samples > 0) {
        glEnable(GL_MULTISAMPLE);
    }
}


void SimpleGLWidget::checkGLState()
{
    if (!isValid()) {
        qDebug() << "Context not valid yet.";
        return;
    }

    makeCurrent(); // Активируем контекст

    GLint samples = 0;
    glGetIntegerv(GL_SAMPLES, &samples); // Читаем количество сэмплов текущего FBO

    qDebug() << "Actual FBO samples:" << samples;
    qDebug() << "Vendor:" << (const char*)glGetString(GL_VENDOR);
    qDebug() << "Renderer:" << (const char*)glGetString(GL_RENDERER);

    doneCurrent();
}
