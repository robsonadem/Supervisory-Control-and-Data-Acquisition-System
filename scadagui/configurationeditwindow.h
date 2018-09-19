#ifndef CONFIGURATIONEDITWINDOW_H
#define CONFIGURATIONEDITWINDOW_H

#include "../DMS/dms.h"
#include "../DMS/table.h"
#include <ostream>
#include <fstream>
#include <istream>
#include <QWidget>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QProgressBar>
#include <QWidget>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QProgressBar>
#include <QTextStream>
#include <QTextEdit>
#include "../scada/ControlSubSystem/branch.h"
#include "../scada/ControlSubSystem/modemanager.h"
#include "../scada/ControlSubSystem/state.h"

namespace Ui {
class ConfigurationEditWindow;
}

class ConfigurationEditWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigurationEditWindow(QWidget *parent = 0);    
    explicit ConfigurationEditWindow(QWidget *parent = 0, DMS * db = 0);
//    explicit ConfigurationEditWindow(QWidget *parent = 0, DMS * db = 0, QTextEdit * edit = 0);
    ~ConfigurationEditWindow();

private slots:
    void on_getfileButton_clicked();

    void on_Savebutton_clicked();

    void on_listWidget_itemSelectionChanged();

private:
    Ui::ConfigurationEditWindow *ui;
    bool checkModeManagerConfigure(string file);
    bool checkSensorConfigure(string file);
    DMS * db;    
    ModeManager *m;
//    QTextEdit * edit;
};

#endif // CONFIGURATIONEDITWINDOW_H
