#include "delbutton.h"

DelButton::DelButton(QString text, int index) : QPushButton(text)
{
    this->index = index;
    connect(this, SIGNAL(clicked()), this, SLOT(myClicked()));
}

void DelButton::myClicked()
{
    emit clickedIndex(index);
}
