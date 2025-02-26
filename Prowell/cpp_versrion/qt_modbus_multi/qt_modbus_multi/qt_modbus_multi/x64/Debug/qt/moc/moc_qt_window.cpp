/****************************************************************************
** Meta object code from reading C++ file 'qt_window.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../qt_window.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qt_window.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN9qt_windowE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN9qt_windowE = QtMocHelpers::stringData(
    "qt_window",
    "mainWindowDisplayPage",
    "",
    "mainPageIndex",
    "openSiteSettingWindow",
    "siteSettingWindowDisplayPage",
    "siteSettingpageIndex",
    "siteSettingWindowSave",
    "refreshMainWindow",
    "openCommSettingsWindow",
    "monitoringIndex",
    "openMonitoringWindow",
    "connectToSlave",
    "index",
    "disconnectFromSlave",
    "readFromSlave",
    "updateStatus",
    "QModbusDevice::State",
    "state"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN9qt_windowE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   80,    2, 0x08,    1 /* Private */,
       4,    0,   83,    2, 0x08,    3 /* Private */,
       5,    1,   84,    2, 0x08,    4 /* Private */,
       7,    0,   87,    2, 0x08,    6 /* Private */,
       8,    0,   88,    2, 0x08,    7 /* Private */,
       9,    1,   89,    2, 0x08,    8 /* Private */,
      11,    1,   92,    2, 0x08,   10 /* Private */,
      12,    1,   95,    2, 0x08,   12 /* Private */,
      14,    1,   98,    2, 0x08,   14 /* Private */,
      15,    1,  101,    2, 0x08,   16 /* Private */,
      16,    2,  104,    2, 0x08,   18 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   10,
    QMetaType::Void, QMetaType::Int,   10,
    QMetaType::Void, QMetaType::Int,   13,
    QMetaType::Void, QMetaType::Int,   13,
    QMetaType::Void, QMetaType::Int,   13,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 17,   13,   18,

       0        // eod
};

Q_CONSTINIT const QMetaObject qt_window::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_ZN9qt_windowE.offsetsAndSizes,
    qt_meta_data_ZN9qt_windowE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN9qt_windowE_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<qt_window, std::true_type>,
        // method 'mainWindowDisplayPage'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'openSiteSettingWindow'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'siteSettingWindowDisplayPage'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'siteSettingWindowSave'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'refreshMainWindow'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'openCommSettingsWindow'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'openMonitoringWindow'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'connectToSlave'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'disconnectFromSlave'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'readFromSlave'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'updateStatus'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<QModbusDevice::State, std::false_type>
    >,
    nullptr
} };

void qt_window::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<qt_window *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->mainWindowDisplayPage((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->openSiteSettingWindow(); break;
        case 2: _t->siteSettingWindowDisplayPage((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->siteSettingWindowSave(); break;
        case 4: _t->refreshMainWindow(); break;
        case 5: _t->openCommSettingsWindow((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->openMonitoringWindow((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 7: _t->connectToSlave((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 8: _t->disconnectFromSlave((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 9: _t->readFromSlave((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 10: _t->updateStatus((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QModbusDevice::State>>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject *qt_window::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *qt_window::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN9qt_windowE.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int qt_window::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 11;
    }
    return _id;
}
QT_WARNING_POP
