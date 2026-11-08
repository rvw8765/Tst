#include "ProgressBarDelegate.h"
#include <QApplication>

namespace {
struct Progress {
    int current = 0;
    int total = 0;
    bool isValid = false;
    constexpr Progress() = default;
};

Progress getProgress(const QVariant &data)
{
    Progress result;
    if ( !data.isValid() ) return result;
    auto list = data.toString().split(';');
    if (list.count() != 2) return result;
    bool ok = false;
    result.current = list.first().toInt(&ok);
    if ( !ok ) return result;
    result.total = list.last().toInt(&ok);
    if ( !ok ) return result;
    result.isValid = result.current >= 0 && result.total > 0 && result.total >= result.current;
    return result;
}

}

void ProgressBarDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyle *style = option.widget ? option.widget->style() : QApplication::style();
    QStyleOptionViewItem itemOption = option;
    initStyleOption(&itemOption, index);
    itemOption.text.clear();
    style->drawControl(QStyle::CE_ItemViewItem, &itemOption, painter, itemOption.widget);

    auto progress = getProgress(index.data(Qt::DisplayRole));
    if ( !progress.isValid ) return;

    QStyleOptionProgressBar progressBarOption;
    progressBarOption.rect = option.rect.adjusted(1, 1, -1, -1);
    progressBarOption.minimum = 0;
    progressBarOption.maximum = progress.total;
    progressBarOption.progress = progress.current;
    progressBarOption.text = QString::number(progress.current * 100. / progress.total, 'f', 1) + "%";

    progressBarOption.textVisible = true;
    progressBarOption.textAlignment = Qt::AlignCenter;
    progressBarOption.state = QStyle::State_Enabled;

        // progressBarOption.maximum = 0;
        // progressBarOption.progress = 0;
        // progressBarOption.text = "нет данных";

    style->drawControl(QStyle::CE_ProgressBar, &progressBarOption, painter, option.widget);
}
