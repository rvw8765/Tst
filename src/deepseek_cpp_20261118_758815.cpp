#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>

class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    MyGLWidget(QWidget *parent = nullptr) : QOpenGLWidget(parent) {}

protected:
    void initializeGL() override {
        initializeOpenGLFunctions();

        // ВАЖНО: отключаем встроенный MSAA, чтобы не было двойной работы и конфликтов
        QSurfaceFormat fmt = format();
        fmt.setSamples(0);
        setFormat(fmt);
    }

    void resizeGL(int w, int h) override {
        // Пересоздаём FBO при изменении размера, чтобы он соответствовал новым габаритам
        createFBOs(w, h);
    }

    void paintGL() override {
        if (!m_msaaFbo || !m_resolveFbo) return;

        // 1. Рендерим сцену в мультисэмплированный FBO
        m_msaaFbo->bind();
        // Настраиваем viewport и очищаем буферы
        glViewport(0, 0, m_msaaFbo->width(), m_msaaFbo->height());
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ... Здесь ваш код рендеринга сцены ...

        // 2. Разрешаем (resolve) мультисэмплированный FBO в обычный
        m_msaaFbo->release();

        QOpenGLFramebufferObject::blitFramebuffer(m_resolveFbo, m_msaaFbo);

        // 3. Выводим результат на экран
        // QOpenGLWidget автоматически привяжет свой FBO перед paintGL, но мы его перезаписали.
        // Теперь нам нужно скопировать содержимое нашего resolve FBO в FBO виджета.
        // Но проще сделать это через QPainter или прямой blit в defaultFramebufferObject().
        
        // Вариант А: Прямой blit в FBO виджета
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, defaultFramebufferObject());
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_resolveFbo->handle());
        glBlitFramebuffer(0, 0, m_resolveFbo->width(), m_resolveFbo->height(),
                          0, 0, width(), height(),
                          GL_COLOR_BUFFER_BIT, GL_NEAREST);
        
        // Вариант Б (если нужно смешать с QPainter): использовать QPainter на виджете
        // QPainter painter(this);
        // painter.drawImage(0, 0, m_resolveFbo->toImage());
    }

private:
    void createFBOs(int w, int h) {
        // Учитываем devicePixelRatio для HiDPI
        int pixelW = w * devicePixelRatioF();
        int pixelH = h * devicePixelRatioF();

        // Создаём мультисэмплированный FBO
        QOpenGLFramebufferObjectFormat msaaFormat;
        msaaFormat.setSamples(4); // Запрашиваем 4 выборки
        msaaFormat.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        m_msaaFbo.reset(new QOpenGLFramebufferObject(pixelW, pixelH, msaaFormat));

        // Создаём обычный FBO для разрешения (без MSAA)
        QOpenGLFramebufferObjectFormat resolveFormat;
        resolveFormat.setSamples(0);
        resolveFormat.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        m_resolveFbo.reset(new QOpenGLFramebufferObject(pixelW, pixelH, resolveFormat));
    }

    QScopedPointer<QOpenGLFramebufferObject> m_msaaFbo;
    QScopedPointer<QOpenGLFramebufferObject> m_resolveFbo;
};