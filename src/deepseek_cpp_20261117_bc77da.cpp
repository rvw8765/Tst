#include "FreezeTableWidget.h"

#include <QHeaderView>
#include <QScrollBar>
#include <QItemSelectionModel>
#include <QResizeEvent>

FreezeTableWidget::FreezeTableWidget(QWidget *parent)
    : QTableView(parent)
{
    initFrozenView();

    connect(verticalScrollBar(), &QScrollBar::valueChanged,
            this, &FreezeTableWidget::syncFrozenScroll);
    connect(m_frozenView->verticalScrollBar(), &QScrollBar::valueChanged,
            verticalScrollBar(), &QScrollBar::setValue);

    connect(horizontalHeader(), &QHeaderView::sectionResized,
            this, &FreezeTableWidget::updateFrozenSectionWidth);
    connect(verticalHeader(), &QHeaderView::sectionResized,
            this, &FreezeTableWidget::updateFrozenSectionHeight);
}

FreezeTableWidget::~FreezeTableWidget() = default;

void FreezeTableWidget::initFrozenView()
{
    m_frozenView = new QTableView(this);
    m_frozenView->setFocusPolicy(Qt::NoFocus);
    m_frozenView->setFrameStyle(QFrame::NoFrame);
    m_frozenView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_frozenView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_frozenView->setSelectionBehavior(selectionBehavior());
    m_frozenView->setSelectionMode(selectionMode());
    m_frozenView->setEditTriggers(editTriggers());
    m_frozenView->verticalHeader()->hide();
    m_frozenView->setWordWrap(wordWrap());
    m_frozenView->horizontalHeader()->setVisible(false);
    m_frozenView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_frozenView->hide();
}

void FreezeTableWidget::setModel(QAbstractItemModel *model)
{
    QTableView::setModel(model);
    m_frozenView->setModel(model);

    if (model) {
        m_frozenView->setSelectionModel(selectionModel());

        if (itemDelegate() && itemDelegate() != m_frozenView->itemDelegate())
            m_frozenView->setItemDelegate(itemDelegate());

        m_frozenView->horizontalHeader()->setDefaultSectionSize(
            horizontalHeader()->defaultSectionSize());
    }

    // Применяем ранее сохранённое намерение (или 0, если не задавали).
    // applyFrozenVisibility сама ограничит значение по текущей модели.
    applyFrozenVisibility();
    updateFrozenTableGeometry();

    if (viewport())
        viewport()->stackUnder(m_frozenView);
}

void FreezeTableWidget::setFrozenColumnCount(int count)
{
    if (count < 0)
        count = 0;

    // Если модель уже есть — ограничиваем значением columnCount().
    // Если модели нет — сохраняем как есть; окончательное ограничение
    // произойдёт в applyFrozenVisibility() при появлении модели.
    if (model()) {
        const int max = model()->columnCount();
        m_frozenColumnCount = qBound(0, count, max);
    } else {
        m_frozenColumnCount = count;
    }

    applyFrozenVisibility();
    updateFrozenTableGeometry();
}

void FreezeTableWidget::applyFrozenVisibility()
{
    // Без модели нечего применять — намерение сохранено в m_frozenColumnCount
    if (!model()) {
        return;
    }

    const int cols = model()->columnCount();

    // Дополнительно страхуемся: значение могло быть задано до модели
    // и оказаться больше, чем столбцов в модели.
    if (m_frozenColumnCount > cols) {
        m_frozenColumnCount = cols;
    }

    for (int c = 0; c < cols; ++c) {
        const bool frozen = c < m_frozenColumnCount;
        setColumnHidden(c, frozen);
        m_frozenView->setColumnHidden(c, !frozen);
    }

    if (m_frozenColumnCount == 0) {
        m_frozenView->hide();
    } else {
        m_frozenView->show();
    }

    for (int c = 0; c < cols; ++c) {
        m_frozenView->setColumnWidth(c, columnWidth(c));
    }
}

void FreezeTableWidget::updateFrozenSectionWidth(int logicalIndex,
                                                 int /*oldSize*/,
                                                 int newSize)
{
    if (!m_frozenView)
        return;

    if (logicalIndex < m_frozenColumnCount) {
        m_frozenView->setColumnWidth(logicalIndex, newSize);
        updateFrozenTableGeometry();
    }
}

void FreezeTableWidget::updateFrozenSectionHeight(int logicalIndex,
                                                  int /*oldSize*/,
                                                  int newSize)
{
    if (!m_frozenView)
        return;

    m_frozenView->setRowHeight(logicalIndex, newSize);
}

void FreezeTableWidget::updateFrozenTableGeometry()
{
    if (m_updatingGeometry || !m_frozenView)
        return;

    if (m_frozenColumnCount == 0 || !model()) {
        m_frozenView->setGeometry(0, 0, 0, 0);
        return;
    }

    m_updatingGeometry = true;

    int width = 0;
    for (int c = 0; c < m_frozenColumnCount && c < model()->columnCount(); ++c)
        width += columnWidth(c);

    const int vhWidth = verticalHeader()->isVisible() ? verticalHeader()->width() : 0;
    const int x = vhWidth + frameWidth();
    const int y = frameWidth();
    const int h = viewport()->height() + horizontalHeader()->height();

    m_frozenView->setGeometry(x, y, width, h);
    m_frozenView->verticalScrollBar()->setValue(verticalScrollBar()->value());

    m_updatingGeometry = false;
}

void FreezeTableWidget::syncFrozenScroll(int value)
{
    if (m_frozenView)
        m_frozenView->verticalScrollBar()->setValue(value);
}

void FreezeTableWidget::resizeEvent(QResizeEvent *event)
{
    QTableView::resizeEvent(event);
    updateFrozenTableGeometry();
}

void FreezeTableWidget::scrollTo(const QModelIndex &index, ScrollHint hint)
{
    if (!index.isValid())
        return;

    if (index.column() < m_frozenColumnCount && m_frozenView) {
        m_frozenView->scrollTo(index, hint);
        verticalScrollBar()->setValue(m_frozenView->verticalScrollBar()->value());
    } else {
        QTableView::scrollTo(index, hint);
    }
}