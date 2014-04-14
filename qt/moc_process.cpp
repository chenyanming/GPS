/****************************************************************************
** Meta object code from reading C++ file 'process.h'
**
** Created: Mon Apr 14 15:52:26 2014
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "process.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'process.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Process[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
       9,    8,    8,    8, 0x08,
      19,    8,    8,    8, 0x08,
      32,    8,    8,    8, 0x08,
      43,    8,    8,    8, 0x08,
      50,    8,    8,    8, 0x08,
      59,    8,    8,    8, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Process[] = {
    "Process\0\0openGPS()\0readOutput()\0"
    "closeGPS()\0halt()\0reboot()\0click_combo()\0"
};

const QMetaObject Process::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Process,
      qt_meta_data_Process, 0 }
};

const QMetaObject *Process::metaObject() const
{
    return &staticMetaObject;
}

void *Process::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Process))
        return static_cast<void*>(const_cast< Process*>(this));
    return QWidget::qt_metacast(_clname);
}

int Process::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: openGPS(); break;
        case 1: readOutput(); break;
        case 2: closeGPS(); break;
        case 3: halt(); break;
        case 4: reboot(); break;
        case 5: click_combo(); break;
        default: ;
        }
        _id -= 6;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
