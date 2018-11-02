#-------------------------------------------------
#
# Project created by QtCreator 2018-01-04T21:51:27
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
qtHaveModule(printsupport): QT += printsupport

TARGET = GwStudentResultManager
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    GwAssessmentScoreForm.cpp \
    GwConnectionDialog.cpp \
    GwFirstWindow.cpp \
    GwMainWindow.cpp \
    GwStudent.cpp \
    GwStudentView.cpp \
    main.cpp \
    GwGradeBookWidget.cpp \
    GwQueryDialog.cpp \
    GwResult.cpp \
    GwResultSheet.cpp \
    GwStudentData.cpp \
    GwGradeBookScores.cpp

HEADERS  += \
    GwAssessmentScoreForm.h \
    GwConnectionDialog.h \
    GwFirstWindow.h \
    GwMainWindow.h \
    GwStudent.h \
    GwStudentView.h \
    GwGradeBookWidget.h \
    GwQueryDialog.h \
    GwResult.h \
    GwResultSheet.h \
    GwStudentData.h \
    GwGradeBookScores.h

FORMS += \
    GwAssessmentScoreForm.ui \
    GwConnectionDialog.ui \
    GwFirstWindow.ui \
    GwMainWindow.ui \
    GwStudent.ui \
    GwStudentView.ui \
    GwQueryDialog.ui

RESOURCES += \
    images.qrc \
    images.qrc

RC_ICONS += \
        srm.ico

DISTFILES +=

