/****************************************************************************
** Meta object code from reading C++ file 'bptSegmentForm.h'
**
** Created: Tue Aug 30 00:49:31 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "bptSegmentForm.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'bptSegmentForm.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_BptSegmentForm[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      20,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x08,
      44,   15,   15,   15, 0x08,
      73,   15,   15,   15, 0x08,
      94,   15,   15,   15, 0x08,
     122,   15,   15,   15, 0x08,
     150,   15,   15,   15, 0x08,
     197,  191,   15,   15, 0x08,
     237,  191,   15,   15, 0x08,
     276,   15,   15,   15, 0x08,
     301,   15,   15,   15, 0x08,
     316,   15,   15,   15, 0x08,
     360,  333,   15,   15, 0x08,
     414,   15,   15,   15, 0x08,
     443,   15,   15,   15, 0x08,
     474,   15,   15,   15, 0x08,
     499,   15,   15,   15, 0x08,
     529,   15,   15,   15, 0x08,
     556,   15,   15,   15, 0x08,
     585,   15,   15,   15, 0x08,
     611,   15,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_BptSegmentForm[] = {
    "BptSegmentForm\0\0on_btnFileBrowser_clicked()\0"
    "on_btnSegmentStart_clicked()\0"
    "on_btnView_clicked()\0on_btnAutoPruning_clicked()\0"
    "on_btnNoThreshold_clicked()\0"
    "on_btnShowManualClassification_clicked()\0"
    "value\0on_slideSegmentNumber_valueChanged(int)\0"
    "on_spinSegmentNumber_valueChanged(int)\0"
    "on_btnClassify_clicked()\0refreshImage()\0"
    "setCoefficient()\0srcImage,segmentedImageMap\0"
    "devideImage(Image<U8C3>const&,map<int,Image<U8C3>*>*)\0"
    "on_btnLearningFile_clicked()\0"
    "on_btnSegFileBrowser_clicked()\0"
    "on_btnCompaire_clicked()\0"
    "on_btnTreeInspector_clicked()\0"
    "on_btnTreeViewer_clicked()\0"
    "on_btnClassSegment_clicked()\0"
    "on_btnSaveImage_clicked()\0"
    "on_btnClassificationMethod_clicked()\0"
};

const QMetaObject BptSegmentForm::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_BptSegmentForm,
      qt_meta_data_BptSegmentForm, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &BptSegmentForm::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *BptSegmentForm::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *BptSegmentForm::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_BptSegmentForm))
        return static_cast<void*>(const_cast< BptSegmentForm*>(this));
    if (!strcmp(_clname, "Ui::BptSegmentForm"))
        return static_cast< Ui::BptSegmentForm*>(const_cast< BptSegmentForm*>(this));
    return QDialog::qt_metacast(_clname);
}

int BptSegmentForm::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_btnFileBrowser_clicked(); break;
        case 1: on_btnSegmentStart_clicked(); break;
        case 2: on_btnView_clicked(); break;
        case 3: on_btnAutoPruning_clicked(); break;
        case 4: on_btnNoThreshold_clicked(); break;
        case 5: on_btnShowManualClassification_clicked(); break;
        case 6: on_slideSegmentNumber_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: on_spinSegmentNumber_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: on_btnClassify_clicked(); break;
        case 9: refreshImage(); break;
        case 10: setCoefficient(); break;
        case 11: devideImage((*reinterpret_cast< Image<U8C3>const(*)>(_a[1])),(*reinterpret_cast< map<int,Image<U8C3>*>*(*)>(_a[2]))); break;
        case 12: on_btnLearningFile_clicked(); break;
        case 13: on_btnSegFileBrowser_clicked(); break;
        case 14: on_btnCompaire_clicked(); break;
        case 15: on_btnTreeInspector_clicked(); break;
        case 16: on_btnTreeViewer_clicked(); break;
        case 17: on_btnClassSegment_clicked(); break;
        case 18: on_btnSaveImage_clicked(); break;
        case 19: on_btnClassificationMethod_clicked(); break;
        default: ;
        }
        _id -= 20;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
