#ifndef SIMPLEGLWIDGET_H
#define SIMPLEGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>

class SimpleGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit SimpleGLWidget(QWidget* parent = nullptr);
    Q_SLOT void checkGLState();
protected:
    void initializeGL() override;
    void paintGL() override;
};

#endif // SIMPLEGLWIDGET_H
