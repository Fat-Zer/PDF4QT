//    Copyright (C) 2020 Jakub Melka
//
//    This file is part of PdfForQt.
//
//    PdfForQt is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    PdfForQt is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with PDFForQt.  If not, see <https://www.gnu.org/licenses/>.

#include "dimensionsplugin.h"
#include "pdfdrawwidget.h"
#include "pdfwidgetutils.h"
#include "settingsdialog.h"

#include <QPainter>
#include <QFontMetricsF>

namespace pdfplugin
{

DimensionsPlugin::DimensionsPlugin() :
    pdf::PDFPlugin(nullptr),
    m_dimensionTools(),
    m_showDimensionsAction(nullptr),
    m_clearDimensionsAction(nullptr),
    m_settingsAction(nullptr)
{

}

void DimensionsPlugin::setWidget(pdf::PDFWidget* widget)
{
    Q_ASSERT(!m_widget);

    BaseClass::setWidget(widget);

    QAction* horizontalDimensionAction = new QAction(QIcon(":/pdfplugins/dimensiontool/linear-horizontal.svg"), tr("Horizontal Dimension"), this);
    QAction* verticalDimensionAction = new QAction(QIcon(":/pdfplugins/dimensiontool/linear-vertical.svg"), tr("Vertical Dimension"), this);
    QAction* linearDimensionAction = new QAction(QIcon(":/pdfplugins/dimensiontool/linear.svg"), tr("Linear Dimension"), this);
    QAction* perimeterDimensionAction = new QAction(QIcon(":/pdfplugins/dimensiontool/perimeter.svg"), tr("Perimeter"), this);
    QAction* areaDimensionAction = new QAction(QIcon(":/pdfplugins/dimensiontool/area.svg"), tr("Area"), this);

    horizontalDimensionAction->setObjectName("dimensiontool_LinearHorizontalAction");
    verticalDimensionAction->setObjectName("dimensiontool_LinearVerticalAction");
    linearDimensionAction->setObjectName("dimensiontool_LinearAction");
    perimeterDimensionAction->setObjectName("dimensiontool_PerimeterAction");
    areaDimensionAction->setObjectName("dimensiontool_AreaAction");

    horizontalDimensionAction->setCheckable(true);
    verticalDimensionAction->setCheckable(true);
    linearDimensionAction->setCheckable(true);
    perimeterDimensionAction->setCheckable(true);
    areaDimensionAction->setCheckable(true);

    m_dimensionTools[DimensionTool::LinearHorizontal] = new DimensionTool(DimensionTool::LinearHorizontal, widget->getDrawWidgetProxy(), horizontalDimensionAction, this);
    m_dimensionTools[DimensionTool::LinearVertical] = new DimensionTool(DimensionTool::LinearVertical, widget->getDrawWidgetProxy(), verticalDimensionAction, this);
    m_dimensionTools[DimensionTool::Linear] = new DimensionTool(DimensionTool::Linear, widget->getDrawWidgetProxy(), linearDimensionAction, this);
    m_dimensionTools[DimensionTool::Perimeter] = new DimensionTool(DimensionTool::Perimeter, widget->getDrawWidgetProxy(), perimeterDimensionAction, this);
    m_dimensionTools[DimensionTool::Area] = new DimensionTool(DimensionTool::Area, widget->getDrawWidgetProxy(), areaDimensionAction, this);

    pdf::PDFToolManager* toolManager = widget->getToolManager();
    for (DimensionTool* tool : m_dimensionTools)
    {
        toolManager->addTool(tool);
        connect(tool, &DimensionTool::dimensionCreated, this, &DimensionsPlugin::onDimensionCreated);
    }

    m_showDimensionsAction = new QAction(QIcon(":/pdfplugins/dimensiontool/show-dimensions.svg"), tr("Show Dimensions"), this);
    m_clearDimensionsAction = new QAction(QIcon(":/pdfplugins/dimensiontool/clear-dimensions.svg"), tr("Clear Dimensions"), this);
    m_settingsAction = new QAction(QIcon(":/pdfplugins/dimensiontool/settings.svg"), tr("Settings"), this);

    m_showDimensionsAction->setCheckable(true);
    m_showDimensionsAction->setChecked(true);

    connect(m_showDimensionsAction, &QAction::triggered, this, &DimensionsPlugin::onShowDimensionsTriggered);
    connect(m_clearDimensionsAction, &QAction::triggered, this, &DimensionsPlugin::onClearDimensionsTriggered);
    connect(m_settingsAction, &QAction::triggered, this, &DimensionsPlugin::onSettingsTriggered);

    m_lengthUnit = DimensionUnit::getLengthUnits().front();
    m_areaUnit = DimensionUnit::getAreaUnits().front();
    m_angleUnit = DimensionUnit::getAngleUnits().front();

    m_widget->getDrawWidgetProxy()->registerDrawInterface(this);

    updateActions();
}

void DimensionsPlugin::setDocument(const pdf::PDFModifiedDocument& document)
{
    BaseClass::setDocument(document);

    if (document.hasReset())
    {
        m_dimensions.clear();
        updateActions();
    }
}

std::vector<QAction*> DimensionsPlugin::getActions() const
{
    std::vector<QAction*> result;

    for (DimensionTool* tool : m_dimensionTools)
    {
        if (tool)
        {
            result.push_back(tool->getAction());
        }
    }

    if (!result.empty())
    {
        result.push_back(nullptr);
        result.push_back(m_showDimensionsAction);
        result.push_back(m_clearDimensionsAction);
        result.push_back(m_settingsAction);
    }

    return result;
}


void DimensionsPlugin::drawPage(QPainter* painter,
                                pdf::PDFInteger pageIndex,
                                const pdf::PDFPrecompiledPage* compiledPage,
                                pdf::PDFTextLayoutGetter& layoutGetter,
                                const QMatrix& pagePointToDevicePointMatrix,
                                QList<pdf::PDFRenderError>& errors) const
{
    Q_UNUSED(compiledPage);
    Q_UNUSED(layoutGetter);
    Q_UNUSED(errors);

    if (!m_showDimensionsAction || !m_showDimensionsAction->isChecked() || m_dimensions.empty())
    {
        // Nothing to draw
        return;
    }

    QLocale locale;
    for (const Dimension& dimension : m_dimensions)
    {
        if (pageIndex != dimension.getPageIndex())
        {
            continue;
        }

        switch (dimension.getType())
        {
            case Dimension::Linear:
            {
                QPointF p1 = pagePointToDevicePointMatrix.map(dimension.getPolygon().front());
                QPointF p2 = pagePointToDevicePointMatrix.map(dimension.getPolygon().back());

                // Swap so p1 is to the left of the page, before p2 (for correct determination of angle)
                if (p1.x() > p2.x())
                {
                    qSwap(p1, p2);
                }

                QLineF line(p1, p2);

                if (qFuzzyIsNull(line.length()))
                {
                    // If we have zero line, then do nothing
                    continue;
                }

                QLineF unitVectorLine = line.normalVector().unitVector();
                QPointF unitVector = unitVectorLine.p2() - unitVectorLine.p1();
                qreal extensionLineSize = pdf::PDFWidgetUtils::scaleDPI_y(painter->device(), 5);

                painter->setPen(Qt::black);
                painter->drawLine(line);

                QLineF extensionLineLeft(p1 - unitVector * extensionLineSize, p1 + unitVector * extensionLineSize);
                QLineF extensionLineRight(p2 - unitVector * extensionLineSize, p2 + unitVector * extensionLineSize);

                painter->drawLine(extensionLineLeft);
                painter->drawLine(extensionLineRight);

                QPointF textPoint = line.center();
                qreal angle = line.angle();
                QFontMetricsF fontMetrics(painter->font());
                QRectF textRect(-line.length() * 0.5, -fontMetrics.lineSpacing(), line.length(), fontMetrics.lineSpacing());

                QString text = QString("%1 %2").arg(locale.toString(dimension.getMeasuredValue() * m_lengthUnit.scale, 'f', 2), m_lengthUnit.symbol);

                painter->save();
                painter->translate(textPoint);
                painter->rotate(-angle);
                painter->drawText(textRect, Qt::AlignCenter | Qt::TextDontClip, text);
                painter->restore();

                break;
            }

            case Dimension::Perimeter:
                break;

            case Dimension::Area:
                break;

            default:
                Q_ASSERT(false);
                break;
        }
    }
}

void DimensionsPlugin::onShowDimensionsTriggered()
{
    updateGraphics();
}

void DimensionsPlugin::onClearDimensionsTriggered()
{
    m_dimensions.clear();
    updateActions();
    updateGraphics();
}

void DimensionsPlugin::onSettingsTriggered()
{
    SettingsDialog dialog(m_widget, m_lengthUnit, m_areaUnit, m_angleUnit);
    dialog.exec();

    updateGraphics();
}

void DimensionsPlugin::onDimensionCreated(Dimension dimension)
{
    m_dimensions.emplace_back(qMove(dimension));
    updateActions();
    updateGraphics();
}

void DimensionsPlugin::updateActions()
{
    if (m_showDimensionsAction)
    {
        m_showDimensionsAction->setEnabled(!m_dimensions.empty());
    }
    if (m_clearDimensionsAction)
    {
        m_clearDimensionsAction->setEnabled(!m_dimensions.empty());
    }
}

void DimensionsPlugin::updateGraphics()
{
    if (m_widget)
    {
        m_widget->getDrawWidget()->getWidget()->update();
    }
}

}