/********************************************************************************
** Form generated from reading UI file 'bptSegmentForm.ui'
**
** Created: Tue Aug 30 00:49:33 2011
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BPTSEGMENTFORM_H
#define UI_BPTSEGMENTFORM_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_BptSegmentForm
{
public:
    QLabel *lblImage;
    QPushButton *btnFileBrowser;
    QLabel *lblFileBrowser;
    QLabel *lblClustering;
    QLabel *lblNumberOfClass;
    QSpinBox *spinBox;
    QPushButton *pushButton;
    QPushButton *btnTreeInspector;
    QGroupBox *groupBox;
    QLabel *lblNumberOfRegions;
    QSpinBox *spinNumberOfPruningRegion;
    QPushButton *btnView;
    QGroupBox *groupBox_2;
    QPushButton *btnNoThreshold;
    QLineEdit *txtTextureWavelet;
    QLineEdit *txtMeanGradient;
    QPushButton *btnAutoPruning;
    QLineEdit *txtEvolvementThreshold;
    QCheckBox *chkTextureWavelet;
    QLineEdit *txtMeanColor;
    QCheckBox *chkHomogeneity;
    QLineEdit *txtHomogeneity;
    QLabel *lblEvolvementThreshold;
    QCheckBox *chkMeanColor;
    QCheckBox *chkMeanGradient;
    QLineEdit *txtRegionSize;
    QCheckBox *chkRegionSize;
    QCheckBox *chkColorVariance;
    QLineEdit *txtColorVariance;
    QGroupBox *groupBox_3;
    QPushButton *btnShowManualClassification;
    QLabel *lblLearningFile;
    QPushButton *btnLearningFile;
    QPushButton *btnClassify;
    QPushButton *btnClassificationMethod;
    QGroupBox *groupBox_4;
    QPushButton *btnSegFileBrowser;
    QPushButton *btnCompaire;
    QGroupBox *groupBox_5;
    QPushButton *btnClassSegment;
    QPushButton *btnTreeViewer;
    QPushButton *btnSaveImage;

    void setupUi(QWidget *BptSegmentForm)
    {
        if (BptSegmentForm->objectName().isEmpty())
            BptSegmentForm->setObjectName(QString::fromUtf8("BptSegmentForm"));
        BptSegmentForm->resize(1024, 610);
        lblImage = new QLabel(BptSegmentForm);
        lblImage->setObjectName(QString::fromUtf8("lblImage"));
        lblImage->setGeometry(QRect(260, 60, 481, 381));
        btnFileBrowser = new QPushButton(BptSegmentForm);
        btnFileBrowser->setObjectName(QString::fromUtf8("btnFileBrowser"));
        btnFileBrowser->setGeometry(QRect(130, 10, 85, 27));
        lblFileBrowser = new QLabel(BptSegmentForm);
        lblFileBrowser->setObjectName(QString::fromUtf8("lblFileBrowser"));
        lblFileBrowser->setGeometry(QRect(10, 20, 55, 15));
        lblClustering = new QLabel(BptSegmentForm);
        lblClustering->setObjectName(QString::fromUtf8("lblClustering"));
        lblClustering->setEnabled(false);
        lblClustering->setGeometry(QRect(280, 590, 55, 15));
        lblNumberOfClass = new QLabel(BptSegmentForm);
        lblNumberOfClass->setObjectName(QString::fromUtf8("lblNumberOfClass"));
        lblNumberOfClass->setEnabled(false);
        lblNumberOfClass->setGeometry(QRect(200, 590, 81, 16));
        spinBox = new QSpinBox(BptSegmentForm);
        spinBox->setObjectName(QString::fromUtf8("spinBox"));
        spinBox->setEnabled(false);
        spinBox->setGeometry(QRect(340, 580, 49, 25));
        pushButton = new QPushButton(BptSegmentForm);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setEnabled(false);
        pushButton->setGeometry(QRect(390, 580, 61, 27));
        btnTreeInspector = new QPushButton(BptSegmentForm);
        btnTreeInspector->setObjectName(QString::fromUtf8("btnTreeInspector"));
        btnTreeInspector->setGeometry(QRect(250, 10, 85, 27));
        groupBox = new QGroupBox(BptSegmentForm);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 40, 221, 51));
        lblNumberOfRegions = new QLabel(groupBox);
        lblNumberOfRegions->setObjectName(QString::fromUtf8("lblNumberOfRegions"));
        lblNumberOfRegions->setGeometry(QRect(0, 20, 91, 16));
        spinNumberOfPruningRegion = new QSpinBox(groupBox);
        spinNumberOfPruningRegion->setObjectName(QString::fromUtf8("spinNumberOfPruningRegion"));
        spinNumberOfPruningRegion->setGeometry(QRect(90, 20, 49, 25));
        spinNumberOfPruningRegion->setMinimum(1);
        spinNumberOfPruningRegion->setMaximum(100);
        spinNumberOfPruningRegion->setValue(10);
        btnView = new QPushButton(groupBox);
        btnView->setObjectName(QString::fromUtf8("btnView"));
        btnView->setGeometry(QRect(140, 20, 51, 27));
        groupBox_2 = new QGroupBox(BptSegmentForm);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 90, 231, 261));
        groupBox_2->setFlat(false);
        groupBox_2->setCheckable(false);
        btnNoThreshold = new QPushButton(groupBox_2);
        btnNoThreshold->setObjectName(QString::fromUtf8("btnNoThreshold"));
        btnNoThreshold->setGeometry(QRect(130, 40, 85, 27));
        txtTextureWavelet = new QLineEdit(groupBox_2);
        txtTextureWavelet->setObjectName(QString::fromUtf8("txtTextureWavelet"));
        txtTextureWavelet->setGeometry(QRect(162, 110, 41, 25));
        txtMeanGradient = new QLineEdit(groupBox_2);
        txtMeanGradient->setObjectName(QString::fromUtf8("txtMeanGradient"));
        txtMeanGradient->setGeometry(QRect(162, 140, 41, 25));
        btnAutoPruning = new QPushButton(groupBox_2);
        btnAutoPruning->setObjectName(QString::fromUtf8("btnAutoPruning"));
        btnAutoPruning->setGeometry(QRect(40, 40, 85, 27));
        txtEvolvementThreshold = new QLineEdit(groupBox_2);
        txtEvolvementThreshold->setObjectName(QString::fromUtf8("txtEvolvementThreshold"));
        txtEvolvementThreshold->setGeometry(QRect(0, 40, 41, 25));
        chkTextureWavelet = new QCheckBox(groupBox_2);
        chkTextureWavelet->setObjectName(QString::fromUtf8("chkTextureWavelet"));
        chkTextureWavelet->setGeometry(QRect(0, 110, 141, 20));
        txtMeanColor = new QLineEdit(groupBox_2);
        txtMeanColor->setObjectName(QString::fromUtf8("txtMeanColor"));
        txtMeanColor->setGeometry(QRect(162, 80, 41, 25));
        chkHomogeneity = new QCheckBox(groupBox_2);
        chkHomogeneity->setObjectName(QString::fromUtf8("chkHomogeneity"));
        chkHomogeneity->setGeometry(QRect(0, 200, 131, 20));
        txtHomogeneity = new QLineEdit(groupBox_2);
        txtHomogeneity->setObjectName(QString::fromUtf8("txtHomogeneity"));
        txtHomogeneity->setGeometry(QRect(160, 200, 41, 25));
        lblEvolvementThreshold = new QLabel(groupBox_2);
        lblEvolvementThreshold->setObjectName(QString::fromUtf8("lblEvolvementThreshold"));
        lblEvolvementThreshold->setGeometry(QRect(0, 20, 131, 16));
        chkMeanColor = new QCheckBox(groupBox_2);
        chkMeanColor->setObjectName(QString::fromUtf8("chkMeanColor"));
        chkMeanColor->setGeometry(QRect(0, 80, 121, 20));
        chkMeanColor->setChecked(true);
        chkMeanGradient = new QCheckBox(groupBox_2);
        chkMeanGradient->setObjectName(QString::fromUtf8("chkMeanGradient"));
        chkMeanGradient->setGeometry(QRect(0, 140, 131, 20));
        txtRegionSize = new QLineEdit(groupBox_2);
        txtRegionSize->setObjectName(QString::fromUtf8("txtRegionSize"));
        txtRegionSize->setGeometry(QRect(162, 170, 41, 25));
        chkRegionSize = new QCheckBox(groupBox_2);
        chkRegionSize->setObjectName(QString::fromUtf8("chkRegionSize"));
        chkRegionSize->setGeometry(QRect(0, 170, 121, 20));
        chkColorVariance = new QCheckBox(groupBox_2);
        chkColorVariance->setObjectName(QString::fromUtf8("chkColorVariance"));
        chkColorVariance->setGeometry(QRect(0, 230, 131, 20));
        txtColorVariance = new QLineEdit(groupBox_2);
        txtColorVariance->setObjectName(QString::fromUtf8("txtColorVariance"));
        txtColorVariance->setGeometry(QRect(160, 230, 41, 25));
        groupBox_3 = new QGroupBox(BptSegmentForm);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setGeometry(QRect(10, 370, 231, 101));
        btnShowManualClassification = new QPushButton(groupBox_3);
        btnShowManualClassification->setObjectName(QString::fromUtf8("btnShowManualClassification"));
        btnShowManualClassification->setGeometry(QRect(0, 40, 111, 27));
        lblLearningFile = new QLabel(groupBox_3);
        lblLearningFile->setObjectName(QString::fromUtf8("lblLearningFile"));
        lblLearningFile->setGeometry(QRect(0, 20, 91, 16));
        btnLearningFile = new QPushButton(groupBox_3);
        btnLearningFile->setObjectName(QString::fromUtf8("btnLearningFile"));
        btnLearningFile->setGeometry(QRect(130, 20, 71, 27));
        btnClassify = new QPushButton(groupBox_3);
        btnClassify->setObjectName(QString::fromUtf8("btnClassify"));
        btnClassify->setGeometry(QRect(150, 70, 51, 27));
        btnClassificationMethod = new QPushButton(groupBox_3);
        btnClassificationMethod->setObjectName(QString::fromUtf8("btnClassificationMethod"));
        btnClassificationMethod->setGeometry(QRect(0, 70, 85, 27));
        groupBox_4 = new QGroupBox(BptSegmentForm);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        groupBox_4->setGeometry(QRect(10, 470, 211, 61));
        btnSegFileBrowser = new QPushButton(groupBox_4);
        btnSegFileBrowser->setObjectName(QString::fromUtf8("btnSegFileBrowser"));
        btnSegFileBrowser->setGeometry(QRect(0, 20, 101, 27));
        btnCompaire = new QPushButton(groupBox_4);
        btnCompaire->setObjectName(QString::fromUtf8("btnCompaire"));
        btnCompaire->setGeometry(QRect(110, 20, 85, 27));
        groupBox_5 = new QGroupBox(BptSegmentForm);
        groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
        groupBox_5->setGeometry(QRect(10, 530, 211, 61));
        btnClassSegment = new QPushButton(groupBox_5);
        btnClassSegment->setObjectName(QString::fromUtf8("btnClassSegment"));
        btnClassSegment->setGeometry(QRect(84, 20, 111, 27));
        btnTreeViewer = new QPushButton(BptSegmentForm);
        btnTreeViewer->setObjectName(QString::fromUtf8("btnTreeViewer"));
        btnTreeViewer->setGeometry(QRect(340, 10, 85, 27));
        btnSaveImage = new QPushButton(BptSegmentForm);
        btnSaveImage->setObjectName(QString::fromUtf8("btnSaveImage"));
        btnSaveImage->setGeometry(QRect(440, 10, 85, 27));

        retranslateUi(BptSegmentForm);

        QMetaObject::connectSlotsByName(BptSegmentForm);
    } // setupUi

    void retranslateUi(QWidget *BptSegmentForm)
    {
        BptSegmentForm->setWindowTitle(QApplication::translate("BptSegmentForm", "BPT Segment Form", 0, QApplication::UnicodeUTF8));
        lblImage->setText(QApplication::translate("BptSegmentForm", "TextLabel", 0, QApplication::UnicodeUTF8));
        btnFileBrowser->setText(QApplication::translate("BptSegmentForm", "File browser", 0, QApplication::UnicodeUTF8));
        lblFileBrowser->setText(QApplication::translate("BptSegmentForm", "File", 0, QApplication::UnicodeUTF8));
        lblClustering->setText(QApplication::translate("BptSegmentForm", "Clustering", 0, QApplication::UnicodeUTF8));
        lblNumberOfClass->setText(QApplication::translate("BptSegmentForm", "Class number", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("BptSegmentForm", "Cluster", 0, QApplication::UnicodeUTF8));
        btnTreeInspector->setText(QApplication::translate("BptSegmentForm", "Tree Inspector", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("BptSegmentForm", "Segmentation by region number", 0, QApplication::UnicodeUTF8));
        lblNumberOfRegions->setText(QApplication::translate("BptSegmentForm", "Region Number", 0, QApplication::UnicodeUTF8));
        btnView->setText(QApplication::translate("BptSegmentForm", "View", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("BptSegmentForm", "Segmentation by Evolvement", 0, QApplication::UnicodeUTF8));
        btnNoThreshold->setText(QApplication::translate("BptSegmentForm", "No Threshold", 0, QApplication::UnicodeUTF8));
        txtTextureWavelet->setText(QApplication::translate("BptSegmentForm", "0.5", 0, QApplication::UnicodeUTF8));
        txtMeanGradient->setText(QApplication::translate("BptSegmentForm", "0.5", 0, QApplication::UnicodeUTF8));
        btnAutoPruning->setText(QApplication::translate("BptSegmentForm", "Auto Pruning", 0, QApplication::UnicodeUTF8));
        txtEvolvementThreshold->setText(QApplication::translate("BptSegmentForm", "0.5", 0, QApplication::UnicodeUTF8));
        chkTextureWavelet->setText(QApplication::translate("BptSegmentForm", "Texture Wavelet", 0, QApplication::UnicodeUTF8));
        txtMeanColor->setText(QApplication::translate("BptSegmentForm", "0.5", 0, QApplication::UnicodeUTF8));
        chkHomogeneity->setText(QApplication::translate("BptSegmentForm", "Homogeneity", 0, QApplication::UnicodeUTF8));
        txtHomogeneity->setText(QApplication::translate("BptSegmentForm", "0.5", 0, QApplication::UnicodeUTF8));
        lblEvolvementThreshold->setText(QApplication::translate("BptSegmentForm", "Evolvement Threshold", 0, QApplication::UnicodeUTF8));
        chkMeanColor->setText(QApplication::translate("BptSegmentForm", "Mean color", 0, QApplication::UnicodeUTF8));
        chkMeanGradient->setText(QApplication::translate("BptSegmentForm", "Mean Gradient", 0, QApplication::UnicodeUTF8));
        txtRegionSize->setText(QApplication::translate("BptSegmentForm", "0.5", 0, QApplication::UnicodeUTF8));
        chkRegionSize->setText(QApplication::translate("BptSegmentForm", "Region Size", 0, QApplication::UnicodeUTF8));
        chkColorVariance->setText(QApplication::translate("BptSegmentForm", "Color Variance", 0, QApplication::UnicodeUTF8));
        txtColorVariance->setText(QApplication::translate("BptSegmentForm", "0.5", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("BptSegmentForm", "Classification after segmentation", 0, QApplication::UnicodeUTF8));
        btnShowManualClassification->setText(QApplication::translate("BptSegmentForm", "Manually Classify", 0, QApplication::UnicodeUTF8));
        lblLearningFile->setText(QApplication::translate("BptSegmentForm", "Learning File", 0, QApplication::UnicodeUTF8));
        btnLearningFile->setText(QApplication::translate("BptSegmentForm", "Browse", 0, QApplication::UnicodeUTF8));
        btnClassify->setText(QApplication::translate("BptSegmentForm", "Classify", 0, QApplication::UnicodeUTF8));
        btnClassificationMethod->setText(QApplication::translate("BptSegmentForm", "Method", 0, QApplication::UnicodeUTF8));
        groupBox_4->setTitle(QApplication::translate("BptSegmentForm", "Validation with Seg file", 0, QApplication::UnicodeUTF8));
        btnSegFileBrowser->setText(QApplication::translate("BptSegmentForm", "Seg File Browser", 0, QApplication::UnicodeUTF8));
        btnCompaire->setText(QApplication::translate("BptSegmentForm", "Compaire", 0, QApplication::UnicodeUTF8));
        groupBox_5->setTitle(QApplication::translate("BptSegmentForm", "Segmentation after classification", 0, QApplication::UnicodeUTF8));
        btnClassSegment->setText(QApplication::translate("BptSegmentForm", "Class Segment", 0, QApplication::UnicodeUTF8));
        btnTreeViewer->setText(QApplication::translate("BptSegmentForm", "Tree Viewer", 0, QApplication::UnicodeUTF8));
        btnSaveImage->setText(QApplication::translate("BptSegmentForm", "Save Image", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class BptSegmentForm: public Ui_BptSegmentForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BPTSEGMENTFORM_H
