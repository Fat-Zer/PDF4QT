//    Copyright (C) 2021 Jakub Melka
//
//    This file is part of Pdf4Qt.
//
//    Pdf4Qt is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    with the written consent of the copyright owner, any later version.
//
//    Pdf4Qt is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with Pdf4Qt.  If not, see <https://www.gnu.org/licenses/>.

#ifndef STATISTICSGRAPHWIDGET_H
#define STATISTICSGRAPHWIDGET_H

#include <QWidget>

namespace Ui
{
class StatisticsGraphWidget;
}

namespace pdfplugin
{

class StatisticsGraphWidget : public QWidget
{
    Q_OBJECT

public:

    struct StatisticsItem
    {
        bool operator==(const StatisticsItem&) const = default;

        qreal portion;
        QColor color;
        QStringList texts;
    };

    struct Statistics
    {
        bool operator==(const Statistics&) const = default;

        QString title;
        QStringList headers;
        std::vector<StatisticsItem> items;
    };

    explicit StatisticsGraphWidget(QWidget* parent);
    virtual ~StatisticsGraphWidget() override;

    void setStatistics(Statistics statistics);

private:
    Ui::StatisticsGraphWidget* ui;

    struct GeometryHint
    {
        QMargins margins;
        int colorRectangleWidth = 0;
        int linesWidthLeft = 0;
        int linesWidthRight = 0;

        int titleWidth = 0;
        int titleHeight = 0;
        int textHeight = 0;

        std::vector<int> textWidths;

        QSize minimalWidgetSize = 0;
    };

    GeometryHint getGeometryHint() const;

    QFont getTitleFont() const;
    QFont getHeaderFont() const;
    QFont getTextFont() const;

    Statistics m_statistics;
};

}   // namespace pdfplugin

#endif // STATISTICSGRAPHWIDGET_H