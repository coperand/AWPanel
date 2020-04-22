#include "adddescription.h"

AddDescription::AddDescription(QWidget *parent, NetThread *net, QVector<struct keyEvent> vector) : QWidget(parent)
{
    QVBoxLayout *vertical = new QVBoxLayout();
    QHBoxLayout *horizontal = new QHBoxLayout();
    QLabel *descrLabel = new QLabel("Describe macro:");
    descr = new QTextEdit();
    connect(descr, SIGNAL(textChanged()), this, SLOT(onTextChanged()));

    threshold = new QLabel("Character limit reached");
    threshold->setHidden(true);

    QPushButton *addBtn = new QPushButton("Add");
    QPushButton *cancelBtn = new QPushButton("Cancel");

    qRegisterMetaType <struct macro> ("struct macro");
    connect(addBtn, SIGNAL(clicked()), this, SLOT(addClicked()));
    connect(cancelBtn, SIGNAL(clicked()), parent, SLOT(cancel()));
    connect(this, SIGNAL(returnToMain()), parent, SLOT(cancel()));
    connect(this, SIGNAL(sendMacro(struct macro)), net, SLOT(addMacro(struct macro)));

    horizontal->addWidget(cancelBtn);
    horizontal->addWidget(addBtn);
    vertical->addWidget(descrLabel);
    vertical->setAlignment(descrLabel, Qt::AlignTop);
    vertical->addWidget(descr);
    vertical->setAlignment(descr, Qt::AlignTop);
    vertical->addWidget(threshold);
    vertical->addLayout(horizontal);
    vertical->setStretch(0, 0);
    vertical->setStretch(1, 1);
    vertical->setStretch(2, 0);
    vertical->setStretch(3, 0);

    this->vector = vector;
    code = net->next_code;
    setLayout(vertical);
    descr->setFocus();
}

void AddDescription::onTextChanged()
{
    int max_len = 160;
    if(descr->toPlainText().length() > max_len)
    {
        threshold->setHidden(false);
        descr->setText(descr->toPlainText().left(max_len));
        QTextCursor cursor = descr->textCursor();
        cursor.movePosition(QTextCursor::End);
        descr->setTextCursor(cursor);
    }

}

void AddDescription::addClicked()
{
    if(descr->toPlainText().size() != 0)
    {
        struct macro mac= {code, descr->toPlainText(), vector};
        emit sendMacro(mac);
        emit returnToMain();
    }
}
