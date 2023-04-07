QT += quick
CONFIG += qmltypes
QML_IMPORT_NAME = mycompany.module
QML_IMPORT_MAJOR_VERSION = 1

LIBS += -lmariadbcpp

SOURCES += \
        main.cpp \
        rtmpclient.cpp \
        tcpsocket.cpp \
        videosocialcontrol.cpp

resources.files = qml/main.qml \
    qml/AccountInformation.qml \
    qml/ChangeNickname.qml \
    qml/ChangePassword.qml \
    qml/CommentManger.qml \
    qml/Dialogs.qml \
    qml/Delegate.qml\
    qml/EditManuscript.qml \
    qml/HomePage.qml \
    qml/LoginPage.qml \
    qml/MainPage.qml \
    qml/ManuscriptPage.qml \
    qml/MessageDialogs.qml \
    qml/MyFriend.qml \
    qml/PersonalPage.qml \
    qml/PublishPage.qml \
    qml/QmlBusyIndicator.qml \
    qml/RegisterPage.qml \
    qml/SettingPage.qml \
    qml/TimeDialog.qml \
    qml/WatchVideo.qml

resources.prefix = /$${TARGET}
RESOURCES += resources \
    image/image.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    rtmpclient.h \
    tcpsocket.h \
    videosocialcontrol.h

DISTFILES += \
    qml/AccountInformation.qml \
    qml/ChangeNickname.qml \
    qml/ChangePassword.qml \
    qml/CommentManger.qml \
    qml/Delegate.qml \
    qml/Dialogs.qml \
    qml/EditManuscript.qml \
    qml/FullScreen.qml \
    qml/HomePage.qml \
    qml/LoginPage.qml \
    qml/MainPage.qml \
    qml/ManuscriptPage.qml \
    qml/MessageDialogs.qml \
    qml/MyFriend.qml \
    qml/PersonalPage.qml \
    qml/PublishPage.qml \
    qml/QmlBusyIndicator.qml \
    qml/RegisterPage.qml \
    qml/SettingPage.qml \
    qml/TimeDialog.qml \
    qml/WatchVideo.qml \
    qml/main.qml

unix|win32: LIBS += -lrtmp
unix|win32: LIBS += -lpthread
