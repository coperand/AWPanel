#include "addmacro.h"

AddMacro::AddMacro(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *vertical = new QVBoxLayout();
    QHBoxLayout *horizontal = new QHBoxLayout();
    QLabel *pressLabel = new QLabel("Press any keys to add to the macro");

    QHBoxLayout *switches = new QHBoxLayout();
    ctrl = new Switch("Ctrl");
    win = new Switch("Win");
    alt = new Switch("Alt");
    switches->addWidget(ctrl);
    switches->addWidget(win);
    switches->addWidget(alt);
    connect(ctrl, SIGNAL(stateChanged(int)), this, SLOT(switchChanged(int)));
    connect(win, SIGNAL(stateChanged(int)), this, SLOT(switchChanged(int)));
    connect(alt, SIGNAL(stateChanged(int)), this, SLOT(switchChanged(int)));

    outputLabel = new QLabel("");
    outputLabel->setWordWrap(true);
    outputLabel->setMinimumHeight(150);
    outputLabel->setAlignment(Qt::AlignTop);
    outputLabel->setAutoFillBackground(true);
    QPalette pal = outputLabel->palette();
    pal.setColor(QPalette::Window, QColor(Qt::white));
    outputLabel->setPalette(pal);

    threshold = new QLabel("Character limit reached");
    threshold->setHidden(true);

    QPushButton *nextBtn = new QPushButton("Next");
    QPushButton *cancelBtn = new QPushButton("Cancel");
    nextBtn->setFocusPolicy(Qt::NoFocus);
    cancelBtn->setFocusPolicy(Qt::NoFocus);

    qRegisterMetaType <QVector<struct keyEvent>> ("QVector<struct keyEvent>");
    connect(nextBtn, SIGNAL(clicked()), this, SLOT(nextClicked()));
    connect(cancelBtn, SIGNAL(clicked()), parent, SLOT(cancel()));
    connect(this, SIGNAL(sendVector(QVector<struct keyEvent>)), parent, SLOT(next(QVector<struct keyEvent>)));

    horizontal->addWidget(cancelBtn);
    horizontal->addWidget(nextBtn);

    vertical->addWidget(pressLabel);
    vertical->setAlignment(pressLabel, Qt::AlignTop);
    vertical->addLayout(switches);
    vertical->setAlignment(switches, Qt::AlignTop);
    vertical->addWidget(outputLabel);
    vertical->setAlignment(outputLabel, Qt::AlignTop);
    vertical->addWidget(threshold);
    vertical->addLayout(horizontal);
    vertical->setStretch(0, 0);
    vertical->setStretch(1, 0);
    vertical->setStretch(2, 1);
    vertical->setStretch(3, 0);
    vertical->setStretch(4, 0);

    setFocusPolicy(Qt::StrongFocus);
    setFocus(Qt::OtherFocusReason);
    filled = false;
    setLayout(vertical);
}

void AddMacro::switchChanged(int state)
{
    if(sender() == ctrl)
    {
        if(state == 2)
        {
            if(!filled)
            {
                struct keyEvent item = {static_cast<unsigned short>(VK_CONTROL), false};
                vector.push_back(item);
                pressed.insert(item.code);
            }
        } else
        {
            if( !(vector.back().code == VK_CONTROL && vector.back().up == false) )
            {
                struct keyEvent item = {static_cast<unsigned short>(VK_CONTROL), true};
                vector.push_back(item);
                pressed.remove(item.code);
            } else
                vector.pop_back();
        }
    } else if(sender() == win)
    {
        if(state == 2)
        {
            if(!filled)
            {
                struct keyEvent item = {static_cast<unsigned short>(VK_LWIN), false};
                vector.push_back(item);
                pressed.insert(item.code);
            }
        } else
        {
            if( !(vector.back().code == VK_LWIN && vector.back().up == false) )
            {
                struct keyEvent item = {static_cast<unsigned short>(VK_LWIN), true};
                vector.push_back(item);
                pressed.remove(item.code);
            } else
                vector.pop_back();
        }
    } else if(sender() == alt)
    {
        if(state == 2)
        {
            if(!filled)
            {
                struct keyEvent item = {static_cast<unsigned short>(VK_MENU), false};
                vector.push_back(item);
                pressed.insert(item.code);
            }
        } else
        {
            if( !(vector.back().code == VK_MENU && vector.back().up == false) )
            {
                struct keyEvent item = {static_cast<unsigned short>(VK_MENU), true};
                vector.push_back(item);
                pressed.remove(item.code);
            } else
                vector.pop_back();
        }
    }
}

void AddMacro::showEvent ( QShowEvent * event )
{
    setFocus(Qt::OtherFocusReason);
    event->accept();
}

void AddMacro::nextClicked()
{
    if(vector.count() != 0)
    {
        foreach (unsigned short code, pressed)
        {
            struct keyEvent item = {code, true};
            vector.push_back(item);
        }
        emit sendVector(vector);
    }
}

void AddMacro::keyPressEvent(QKeyEvent *event)
{
    if(!filled)
    {
        int key=event->key();
        if ((key = translateKey(key)) != 0)
        {
            struct keyEvent item = {static_cast<unsigned short>(key), false};
            vector.push_back(item);
            pressed.insert(item.code);
        }
    }
}

void AddMacro::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Tab)
        keyPressEvent(event);

    if(ctrl->isChecked())
        event->modifiers().setFlag(Qt::ControlModifier, false);
    if(win->isChecked())
        event->modifiers().setFlag(Qt::MetaModifier, true);
    if(alt->isChecked())
        event->modifiers().setFlag(Qt::AltModifier, true);

    if(!filled)
    {
        int key=event->key();
        if ((key = translateKey(key)) != 0 )
        {
            struct keyEvent item = {static_cast<unsigned short>(key), true};
            vector.push_back(item);
            pressed.remove(item.code);

            if(key == VK_SHIFT || key == VK_CONTROL || key == VK_MENU || key == VK_LWIN)
            {
                if((vector.count() < 3 || (vector[vector.count() - 2].code == key && vector[vector.count() - 2].up == false)))
                {
                    switch(key)
                    {
                        case VK_SHIFT:
                            outputLabel->setText(outputLabel->text() + "Shift | ");
                            break;
                        case VK_CONTROL:
                            outputLabel->setText(outputLabel->text() + "Ctrl | ");
                            break;
                        case VK_MENU:
                            outputLabel->setText(outputLabel->text() + "Alt | ");
                            break;
                        case VK_LWIN:
                            outputLabel->setText(outputLabel->text() + "Meta | ");
                            break;
                    }
                }
            }
            else
                outputLabel->setText(outputLabel->text() + QKeySequence((event->modifiers() | (ctrl->isChecked() ? Qt::ControlModifier : 0x00) |
                                                                         (win->isChecked() ? Qt::MetaModifier : 0x00) |
                                                                         (alt->isChecked() ? Qt::AltModifier : 0x00)) + event->key()).toString() + " | ");
            if(outputLabel->text().size() > 650)
            {
                filled = true;
                threshold->setHidden(false);
            }

        }
    }
}

int AddMacro::translateKey(int key)
{
    if (!((key>=0x30 && key<=0x39) || (key>=0x41 && key<=0x5A) || key == 0x20))
    {
        switch(key)
        {
            case Qt::Key_Backspace:
                key = VK_BACK;
                break;
            case Qt::Key_Tab:
                key = VK_TAB;
                break;
            case Qt::Key_Return:
                key = VK_RETURN;
                break;
            case Qt::Key_Shift:
                key = VK_SHIFT;
                break;
            case Qt::Key_Control:
                key = VK_CONTROL;
                break;
            case Qt::Key_Alt:
                key = VK_MENU;
                break;
            case Qt::Key_CapsLock:
                key = VK_CAPITAL;
                break;
            case Qt::Key_Escape:
                key = VK_ESCAPE;
                break;
            case Qt::Key_PageUp:
                key = VK_PRIOR;
                break;
            case Qt::Key_PageDown:
                key = VK_NEXT;
                break;
            case Qt::Key_End:
                key = VK_END;
                break;
            case Qt::Key_Home:
                key = VK_HOME;
                break;
            case Qt::Key_Left:
                key = VK_LEFT;
                break;
            case Qt::Key_Up:
                key = VK_UP;
                break;
            case Qt::Key_Right:
                key = VK_RIGHT;
                break;
            case Qt::Key_Down:
                key = VK_DOWN;
                break;
            case Qt::Key_Delete:
                key = VK_DELETE;
                break;
            case Qt::Key_Meta:
                key = VK_LWIN;
                break;
            case Qt::Key_F1:
                key = VK_F1;
                break;
            case Qt::Key_F2:
                key = VK_F2;
                break;
            case Qt::Key_F3:
                key = VK_F3;
                break;
            case Qt::Key_F4:
                key = VK_F4;
                break;
            case Qt::Key_F5:
                key = VK_F5;
                break;
            case Qt::Key_F6:
                key = VK_F6;
                break;
            case Qt::Key_F7:
                key = VK_F7;
                break;
            case Qt::Key_F8:
                key = VK_F8;
                break;
            case Qt::Key_F9:
                key = VK_F9;
                break;
            case Qt::Key_F10:
                key = VK_F10;
                break;
            case Qt::Key_F11:
                key = VK_F11;
                break;
            case Qt::Key_F12:
                key = VK_F12;
                break;
            case Qt::Key_NumLock:
                key = VK_NUMLOCK;
                break;
            case Qt::Key_ScrollLock:
                key = VK_SCROLL;
                break;
            case Qt::Key_Semicolon:
                key = VK_OEM_1;
                break;
            case Qt::Key_Comma:
                key = VK_OEM_COMMA;
                break;
            case Qt::Key_Period:
                key = VK_OEM_PERIOD;
                break;
            case Qt::Key_BracketLeft:
                key = VK_OEM_4;
                break;
            case Qt::Key_BracketRight:
                key = VK_OEM_6;
                break;
            case Qt::Key_Slash:
                key = VK_OEM_2;
                break;
            case Qt::Key_Apostrophe:
                key = VK_OEM_7;
                break;
            case Qt::Key_Backslash:
                key = VK_OEM_5;
                break;
            case Qt::Key_QuoteLeft:
                key = VK_OEM_3;
                break;
            case Qt::Key_Minus:
                key = VK_OEM_MINUS;
                break;
            case Qt::Key_Plus:
                key = VK_ADD;
                break;
            case Qt::Key_Equal:
                key = VK_OEM_PLUS;
                break;
            case Qt::Key_Asterisk:
                key = VK_MULTIPLY;
                break;

            default:
                return 0;
        }
    }

    return key;
}
