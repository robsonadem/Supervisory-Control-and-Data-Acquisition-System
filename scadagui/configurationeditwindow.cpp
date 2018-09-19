#include "configurationeditwindow.h"
#include "ui_configurationeditwindow.h"

ConfigurationEditWindow::ConfigurationEditWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigurationEditWindow)
{
    ui->setupUi(this);
}

ConfigurationEditWindow::ConfigurationEditWindow(QWidget *parent, DMS *db):
    QWidget(parent),
    ui(new Ui::ConfigurationEditWindow)
{
    ui->setupUi(this);
//    this->parent = parent;
    this->db = db;
//    ui->listWidget->addItem(new QListWidgetItem(QString::fromStdString("control_config.txt")));
//    ui->listWidget->addItem(new QListWidgetItem(QString::fromStdString("sensor_config.txt")));
    ui->listWidget->addItem(new QListWidgetItem(QString::fromStdString("mainConfig.txt")));
    ui->FileEditor->setText(QString::fromStdString("<No Contents>"));
}

//ConfigurationEditWindow::ConfigurationEditWindow(QWidget *parent, DMS *db, QTextEdit *edit){
//    ui->setupUi(this);
//    this->edit = edit;
//    this->db = db;
//    ui->listWidget->addItem(new QListWidgetItem(QString::fromStdString("control_config.txt")));
//    ui->listWidget->addItem(new QListWidgetItem(QString::fromStdString("sensor_config.txt")));
//    ui->FileEditor->setText(QString::fromStdString("<No Contents>"));
//}

ConfigurationEditWindow::~ConfigurationEditWindow()
{
    delete ui;
}

//bool ConfigurationEditWindow::checkModeManagerConfigure(string file){
//    string stateDeclaration("STATES:");
//    string branchDeclaration("BRANCHES:");
//    ModeManager m;
//    bool state = false;
//    bool branch = false;
//    if(file.empty()){return false;}
//    vector<char*> list;
//    char * it;
//    it = strtok((char*)file.c_str(), ".\n");
//    while(it != NULL){
//        if(!isdigit(*it))list.push_back(it);
//        it = strtok(NULL, ".\n");
//    }

//    for(int i = 0; i < (int)list.size(); i++){cout << list.at(i) << endl;}

//    for(int i = 0; i < (int)list.size(); i++){
//        string line(list.at(i));
//        if(line.at(0) == '#'){}
//        if(list.size() != 3){return false;}
//        else if(strcmp(list.at(i),stateDeclaration.c_str()) == 0){state = true; branch = false;}
//        else if(!line.compare(branchDeclaration)){state = false;branch = true;}
//        else if (state) {
//            vector<string > stateLine= m.split(line,',');
//            for(string s: stateLine){
//                State tempState(s);
//                m.states.push_back(tempState);
//            }
//        }
//        else if (branch) {
//                vector<string> branchLine= m.split(line,',');
//                for(int i = 0; i < (int)branchLine.size(); i++){cout << branchLine.at(i) << endl;}
//                State from= m.getState(branchLine[0]);
//                State to = m.getState(branchLine[1]);
//                if(!from.name.compare("NULL")){return false;}
//                if(!to.name.compare("NULL")){return false;}
//        }
//    }
//    return true;
//}

void ConfigurationEditWindow::on_getfileButton_clicked()
{
    QListWidgetItem * dummy = ui->listWidget->currentItem();
    string configname = dummy->text().toStdString();
    if(true){
        string dummy = db->getTable("ConfigFileTable")->createQuery("Select contents from ConfigFileTable where fileName = '"
                                                                    +configname + "';");
        vector<char*> list;
        char * it;
        it = strtok((char*)dummy.c_str(), "\n");
        while(it != NULL){
            list.push_back(it);
            it = strtok(NULL, "\n");
        }
        string print;
        for(int i = 0; i < (int)list.size(); i++){
//            print += to_string(i) + ". " + list[i] + "\n";
            print += (string)list[i] + "\n";
        }
        ui->FileEditor->setText(QString::fromStdString(print));
    }
    else{
        ui->FileEditor->setText(QString::fromStdString("Bad Constraints"));
    }
}

void ConfigurationEditWindow::on_Savebutton_clicked()
{
    QListWidgetItem * dummy = ui->listWidget->currentItem();
    string configname = dummy->text().toStdString();

    QString edits = ui->FileEditor->toPlainText();

    QFile file("/Users/robson/scada_repo/configuration_files/mainConfig.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << edits;
//    cout << file.fileName().toStdString();
//    ModeManager m("../configuration_files/control_config.txt", db);
//    m.configure();
//    try{
//        if(configname == "control_config.txt"){m.configure();}
////        db->getTable("ConfigFileTable")->updateTable("filename = " + configname, "'" + edits.toStdString() + "'");
//    }
//    catch(const std::exception &e){
//        QMessageBox::about(this,"Error",e.what());cout << e.what() << endl;
//    }
    string cmd;
    cmd = "contents = '" + edits.toStdString() + "'";    
    db->getTable("ConfigFileTable")->updateTable("filename = '" + configname + "'", cmd);

    db->clearTable("BranchTable");
    db->clearTable("SensorTable");
    db->clearTable("CalConfTable");
    db->clearTable("SampleTable");
    db->clearTable("SensorConfTable");

//    edit->setText(QString::fromStdString(db->getTableHeaders("StateTable")
//                                                  + "\n"
//                                                  + db->getTable("StateTable")->createQuery("Select * from StateTable")));

    this->close();
}

void ConfigurationEditWindow::on_listWidget_itemSelectionChanged()
{
    QListWidgetItem * dummy = ui->listWidget->currentItem();
    string configname = dummy->text().toStdString();
    string wholeTable = db->getTableHeaders("ConfigFileTable")
            + "\n"
            + db->getTable("ConfigFileTable")->createQuery("Select contents from ConfigFileTable where fileName = '"
                                                           +configname + "';");
    ui->FileContentView->setText(QString::fromStdString(wholeTable));
}
