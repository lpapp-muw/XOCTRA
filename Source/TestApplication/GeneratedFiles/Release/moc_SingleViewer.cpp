/****************************************************************************
** Meta object code from reading C++ file 'SingleViewer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../SingleViewer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SingleViewer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_lpmlvis__SingleViewer_t {
    QByteArrayData data[11];
    char stringdata0[153];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_lpmlvis__SingleViewer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_lpmlvis__SingleViewer_t qt_meta_stringdata_lpmlvis__SingleViewer = {
    {
QT_MOC_LITERAL(0, 0, 21), // "lpmlvis::SingleViewer"
QT_MOC_LITERAL(1, 22, 15), // "openFileDialoge"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 16), // "setBlendingValue"
QT_MOC_LITERAL(4, 56, 6), // "aValue"
QT_MOC_LITERAL(5, 63, 15), // "setM0PHighValue"
QT_MOC_LITERAL(6, 79, 15), // "setM1PHighValue"
QT_MOC_LITERAL(7, 95, 9), // "setCursor"
QT_MOC_LITERAL(8, 105, 12), // "PlaneViewer*"
QT_MOC_LITERAL(9, 118, 12), // "aPlaneViewer"
QT_MOC_LITERAL(10, 131, 21) // "setCursorRenderNeeded"

    },
    "lpmlvis::SingleViewer\0openFileDialoge\0"
    "\0setBlendingValue\0aValue\0setM0PHighValue\0"
    "setM1PHighValue\0setCursor\0PlaneViewer*\0"
    "aPlaneViewer\0setCursorRenderNeeded"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_lpmlvis__SingleViewer[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x0a /* Public */,
       3,    1,   45,    2, 0x0a /* Public */,
       5,    1,   48,    2, 0x0a /* Public */,
       6,    1,   51,    2, 0x0a /* Public */,
       7,    1,   54,    2, 0x0a /* Public */,
      10,    1,   57,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, 0x80000000 | 8,    9,

       0        // eod
};

void lpmlvis::SingleViewer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SingleViewer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->openFileDialoge(); break;
        case 1: _t->setBlendingValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->setM0PHighValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->setM1PHighValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->setCursor((*reinterpret_cast< PlaneViewer*(*)>(_a[1]))); break;
        case 5: _t->setCursorRenderNeeded((*reinterpret_cast< PlaneViewer*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< PlaneViewer* >(); break;
            }
            break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< PlaneViewer* >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject lpmlvis::SingleViewer::staticMetaObject = { {
    &QMainWindow::staticMetaObject,
    qt_meta_stringdata_lpmlvis__SingleViewer.data,
    qt_meta_data_lpmlvis__SingleViewer,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *lpmlvis::SingleViewer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *lpmlvis::SingleViewer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_lpmlvis__SingleViewer.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int lpmlvis::SingleViewer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
