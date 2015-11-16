/****************************************************************************
** Meta object code from reading C++ file 'SimpleView4.h'
**
** Created: Sat Apr 14 19:44:08 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../SimpleView/SimpleView4.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SimpleView4.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SimpleView[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x0a,
      23,   11,   11,   11, 0x0a,
      34,   11,   11,   11, 0x0a,
      50,   11,   11,   11, 0x0a,
      73,   69,   11,   11, 0x0a,
     106,   11,   11,   11, 0x0a,
     123,   11,   11,   11, 0x0a,
     146,  144,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_SimpleView[] = {
    "SimpleView\0\0fileOpen()\0fileExit()\0"
    "loadPointFile()\0displayMeanShape()\0"
    "pos\0DisplayShape(vnl_vector<double>)\0"
    "displaySamples()\0clearButtonPressed()\0"
    "i\0spunObjectLoader(int)\0"
};

void SimpleView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SimpleView *_t = static_cast<SimpleView *>(_o);
        switch (_id) {
        case 0: _t->fileOpen(); break;
        case 1: _t->fileExit(); break;
        case 2: _t->loadPointFile(); break;
        case 3: _t->displayMeanShape(); break;
        case 4: _t->DisplayShape((*reinterpret_cast< const vnl_vector<double>(*)>(_a[1]))); break;
        case 5: _t->displaySamples(); break;
        case 6: _t->clearButtonPressed(); break;
        case 7: _t->spunObjectLoader((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData SimpleView::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SimpleView::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_SimpleView,
      qt_meta_data_SimpleView, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SimpleView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SimpleView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SimpleView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SimpleView))
        return static_cast<void*>(const_cast< SimpleView*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int SimpleView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
