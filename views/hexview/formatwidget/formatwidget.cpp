#include "formatwidget.h"
#include "ui_formatwidget.h"

FormatWidget::FormatWidget(QWidget *parent): WorkerTab(parent), ui(new Ui::FormatWidget), _worker(nullptr), _formatmodel(nullptr), _hexedit(nullptr), _logwidget(nullptr), _formatview(nullptr)
{
    ui->setupUi(this);
}

void FormatWidget::setLogWidget(LogWidget *logwidget)
{
    this->_logwidget = logwidget;
}

FormatWidget::~FormatWidget()
{
    delete ui;
}

void FormatWidget::setData(QHexEdit *hexedit)
{
    this->_hexedit = hexedit;
    this->_formatmodel = new FormatModel(this->_hexedit->data(), this);
    ui->tvFormat->setModel(this->_formatmodel);

    connect(ui->tvFormat, SIGNAL(setBackColor(FormatElement*)), this, SLOT(onSetBackColor(FormatElement*)));
    connect(ui->tvFormat, SIGNAL(removeBackColor(FormatElement*)), this, SLOT(onRemoveBackColor(FormatElement*)));
    connect(ui->tvFormat, SIGNAL(formatObjectSelected(FormatElement*)), this, SLOT(onFormatObjectSelected(FormatElement*)));
    connect(ui->tvFormat, SIGNAL(exportAction(FormatElement*)), this, SLOT(exportData(FormatElement*)));
    connect(ui->tvFormat, SIGNAL(importAction(FormatElement*)), this, SLOT(importData(FormatElement*)));
    connect(ui->tvFormat, SIGNAL(gotoOffset(qint64)), this->_hexedit, SLOT(setCursorPos(qint64)));
}

QWidget *FormatWidget::formatView()
{
    return this->_formatview;
}

void FormatWidget::resetData()
{
    this->_formatmodel = new FormatModel(this->_hexedit->data(), this);
    ui->tvFormat->setModel(this->_formatmodel);
}

void FormatWidget::loadFormat()
{
    FormatsDialog fd(this->_hexedit->data()->length(), (this->_hexedit->selectionLength() ? this->_hexedit->cursorPos() : 0), this->topLevelWidget());
    int res = fd.exec();

    if(res == FormatsDialog::Accepted)
    {
        this->_formatview = nullptr;
        this->_logwidget->clear();
        this->_formatdefinition = fd.selectedFormat();

        this->_worker = new FormatWorker(this->_formatdefinition, this->_logwidget, this->_hexedit->data(), fd.offset());
        connect(this->_worker, SIGNAL(started()), this, SIGNAL(workStarted()));
        connect(this->_worker, SIGNAL(started()), this, SIGNAL(parsingStarted()));
        connect(this->_worker, SIGNAL(finished()), this, SIGNAL(workFinished()));
        connect(this->_worker, SIGNAL(finished()), this, SIGNAL(parsingCompleted()));
        connect(this->_worker, SIGNAL(parsingFailed()), this, SIGNAL(parsingFailed()));
        connect(this->_worker, SIGNAL(parsingCompleted()), this, SLOT(onParseCompleted()));
        this->_worker->start();
    }
}

void FormatWidget::onSetBackColor(FormatElement *formatelement)
{
    QColor c = QColorDialog::getColor(Qt::white, this);

    if(c.isValid())
    {
        quint64 offset = formatelement->offset();
        this->_hexedit->highlightBackground(offset, (offset + formatelement->size() - 1), c);
    }
}

void FormatWidget::onRemoveBackColor(FormatElement *formatelement)
{
    quint64 offset = formatelement->offset();
    this->_hexedit->clearHighlight(offset, (offset + formatelement->size() - 1));
}

void FormatWidget::onFormatObjectSelected(FormatElement *formatelement)
{
    quint64 offset = formatelement->offset();
    this->_hexedit->setSelection(offset, offset + formatelement->size());
}

void FormatWidget::exportData(FormatElement *formatelement)
{
    ExportDialog ed(this->_hexedit, this);
    ed.setFixedRange(formatelement->offset(), formatelement->endOffset());
    int res = ed.exec();

    if(res == ExportDialog::Accepted)
        ed.selectedExporter()->callDump(this->_hexedit->data(), ed.fileName(), ed.startOffset(), ed.endOffset());
}

void FormatWidget::importData(FormatElement *formatelement)
{
    QString s = QFileDialog::getOpenFileName(this, "Import binary file...");

    if(!s.isEmpty())
    {
        QFile f(s);
        f.open(QIODevice::ReadOnly);

        quint64 size = qMin(static_cast<quint64>(f.size()), formatelement->size());

        if(size > 0)
        {
            QByteArray ba = f.read(size);
            QHexEditDataWriter writer(this->_hexedit->data());
            writer.replace(formatelement->offset(), size, ba);
        }
    }
}

void FormatWidget::onParseCompleted()
{
    FormatTree* formattree = this->_worker->tree();
    formattree->setParent(this);
    this->_formatmodel->setFormatTree(formattree);

    for(int i = 0; i < this->_formatmodel->columnCount(); i++)
        ui->tvFormat->resizeColumnToContents(i);

    if(this->_formatdefinition->hasView())
        this->_formatview = this->_formatdefinition->callView(this->_hexedit->data(), formattree);
    else
        this->_formatview = nullptr;
}
