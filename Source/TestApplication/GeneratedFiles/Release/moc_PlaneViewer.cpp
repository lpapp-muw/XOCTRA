/****************************************************************************
** Meta object code from reading C++ file 'PlaneViewer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../PlaneViewer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PlaneViewer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_lpmlvis__PlaneViewer_t {
    QByteArrayData data[22];
    char stringdata0[252];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_lpmlvis__PlaneViewer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_lpmlvis__PlaneViewer_t qt_meta_stringdata_lpmlvis__PlaneViewer = {
    {
QT_MOC_LITERAL(0, 0, 20), // "lpmlvis::PlaneViewer"
QT_MOC_LITERAL(1, 21, 13), // "cursorChanged"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 12), // "PlaneViewer*"
QT_MOC_LITERAL(4, 49, 12), // "aPlaneViewer"
QT_MOC_LITERAL(5, 62, 21), // "cursorRender3DToggled"
QT_MOC_LITERAL(6, 84, 6), // "update"
QT_MOC_LITERAL(7, 91, 9), // "setVolume"
QT_MOC_LITERAL(8, 101, 17), // "lpmldata::Volume*"
QT_MOC_LITERAL(9, 119, 7), // "aVolume"
QT_MOC_LITERAL(10, 127, 7), // "setMask"
QT_MOC_LITERAL(11, 135, 5), // "aMask"
QT_MOC_LITERAL(12, 141, 14), // "setOrientation"
QT_MOC_LITERAL(13, 156, 12), // "aOrientation"
QT_MOC_LITERAL(14, 169, 11), // "wheelCursor"
QT_MOC_LITERAL(15, 181, 5), // "aSign"
QT_MOC_LITERAL(16, 187, 11), // "setCursor3D"
QT_MOC_LITERAL(17, 199, 15), // "lpmldata::Index"
QT_MOC_LITERAL(18, 215, 7), // "aCursor"
QT_MOC_LITERAL(19, 223, 14), // "setHovercursor"
QT_MOC_LITERAL(20, 238, 6), // "aPoint"
QT_MOC_LITERAL(21, 245, 6) // "render"

    },
    "lpmlvis::PlaneViewer\0cursorChanged\0\0"
    "PlaneViewer*\0aPlaneViewer\0"
    "cursorRender3DToggled\0update\0setVolume\0"
    "lpmldata::Volume*\0aVolume\0setMask\0"
    "aMask\0setOrientation\0aOrientation\0"
    "wheelCursor\0aSign\0setCursor3D\0"
    "lpmldata::Index\0aCursor\0setHovercursor\0"
    "aPoint\0render"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_lpmlvis__PlaneViewer[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   64,    2, 0x06 /* Public */,
       5,    1,   67,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   70,    2, 0x0a /* Public */,
       7,    1,   71,    2, 0x0a /* Public */,
      10,    1,   74,    2, 0x0a /* Public */,
      12,    1,   77,    2, 0x0a /* Public */,
      14,    1,   80,    2, 0x0a /* Public */,
      16,    1,   83,    2, 0x0a /* Public */,
      19,    1,   86,    2, 0x0a /* Public */,
      21,    0,   89,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, 0x80000000 | 8,   11,
    QMetaType::Void, QMetaType::QString,   13,
    QMetaType::Void, QMetaType::Int,   15,
    QMetaType::Void, 0x80000000 | 17,   18,
    QMetaType::Void, QMetaType::QPoint,   20,
    QMetaType::Void,

       0        // eod
};

void lpmlvis::PlaneViewer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PlaneViewer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->cursorChanged((*reinterpret_cast< PlaneViewer*(*)>(_a[1]))); break;
        case 1: _t->cursorRender3DToggled((*reinterpret_cast< PlaneViewer*(*)>(_a[1]))); break;
        case 2: _t->update(); break;
        case 3: _t->setVolume((*reinterpret_cast< lpmldata::Volume*(*)>(_a[1]))); break;
        case 4: _t->setMask((*reinterpret_cast< lpmldata::Volume*(*)>(_a[1]))); break;
        case 5: _t->setOrientation((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 6: _t->wheelCursor((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->setCursor3D((*reinterpret_cast< const lpmldata::Index(*)>(_a[1]))); break;
        case 8: _t->setHovercursor((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 9: _t->render(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< PlaneViewer* >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< PlaneViewer* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (PlaneViewer::*)(PlaneViewer * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PlaneViewer::cursorChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (PlaneViewer::*)(PlaneViewer * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PlaneViewer::cursorRender3DToggled)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject lpmlvis::PlaneViewer::staticMetaObject = { {
    &QLabel::staticMetaObject,
    qt_meta_stringdata_lpmlvis__PlaneViewer.data,
    qt_meta_data_lpmlvis__PlaneViewer,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *lpmlvis::PlaneViewer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *lpmlvis::PlaneViewer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_lpmlvis__PlaneViewer.stringdata0))
        return static_cast<void*>(this);
    return QLabel::qt_metacast(_clname);
}

int lpmlvis::PlaneViewer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLabel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void lpmlvis::PlaneViewer::cursorChanged(PlaneViewer * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void lpmlvis::PlaneViewer::cursorRender3DToggled(PlaneViewer * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
