#pragma once

#include <QTableView>

class FreezeTableWidget : public QTableView
{
    Q_OBJECT

public:
    explicit FreezeTableWidget(QWidget *parent = nullptr);
    ~FreezeTableWidget() override;

    void setModel(QAbstractItemModel *model) override;

    // Теперь можно вызывать в любой момент — до или после setModel
    void setFrozenColumnCount(int count);
    int frozenColumnCount() const { return m_frozenColumnCount; }

protected:
    void resizeEvent(QResizeEvent *event) override;
    void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible) override;

private slots:
    void updateFrozenSectionWidth(int logicalIndex, int oldSize, int newSize);
    void updateFrozenSectionHeight(int logicalIndex, int oldSize, int newSize);
    void updateFrozenTableGeometry();
    void syncFrozenScroll(int value);

private:
    void initFrozenView();
    void applyFrozenVisibility();

    QTableView *m_frozenView = nullptr;
    int m_frozenColumnCount = 0;
    bool m_updatingGeometry = false;
};