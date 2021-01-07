//    Copyright (C) 2019-2020 Jakub Melka
//
//    This file is part of Pdf4Qt.
//
//    Pdf4Qt is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    Pdf4Qt is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with Pdf4Qt.  If not, see <https://www.gnu.org/licenses/>.

#include "pdfaboutdialog.h"
#include "ui_pdfaboutdialog.h"

#include "pdfutils.h"
#include "pdfwidgetutils.h"

namespace pdfviewer
{

PDFAboutDialog::PDFAboutDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::PDFAboutDialog)
{
    ui->setupUi(this);

    QString html = ui->copyrightLabel->text();
    html.replace("PdfForQtViewer", QApplication::applicationDisplayName());
    ui->copyrightLabel->setText(html);

    std::vector<pdf::PDFDependentLibraryInfo> infos = pdf::PDFDependentLibraryInfo::getLibraryInfo();

    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setRowCount(static_cast<int>(infos.size()));
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << tr("Library") << tr("Version") << tr("License") << tr("URL"));
    ui->tableWidget->setEditTriggers(QTableWidget::NoEditTriggers);
    ui->tableWidget->setSelectionMode(QTableView::SingleSelection);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    for (int i = 0; i < infos.size(); ++i)
    {
        const pdf::PDFDependentLibraryInfo& info = infos[i];
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(info.library));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(info.version));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(info.license));
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(info.url));
    }

    pdf::PDFWidgetUtils::scaleWidget(this, QSize(750, 600));
}

PDFAboutDialog::~PDFAboutDialog()
{
    delete ui;
}

}   // namespace viewer