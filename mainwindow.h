#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QString>
#include <QPushButton>
#include <QLabel>
#include <QTextStream>
#include <QFileInfo>
#include <QDir>
#include <string>
#include <QListWidget>
#include <fstream>
#include <QLineEdit>
#include <vector>
#include <QFile>

#include <QCryptographicHash>
#include <iostream>
#include <AclAPI.h>
#include <windows.h>
#include <cstring>
#include <QInputDialog>


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    std::string path_to_directory;


    QTextEdit *dir_pth_textedit;
    QPushButton *enter_path;
    //QLabel *dir_content_lbl;
    QListWidget *dir_content_list;
    QListWidget *dir_masks;
    QPushButton *security_change_btn;

    QPushButton *delete_mask;
    QPushButton *add_mask;

    QLineEdit *password_linedit_file;

    QLineEdit *password_linedit_mask;
    //QTextStream out;
    std::string file_content;
    std::vector<std::string> directory_files;
    std::vector<std::string> directory_masks;

    int timer;
    int timer_delay;
    bool is_dir_new;
    QDir *dir;
    void show_dir_content();
    //int seek_str_in_fl_since(std::fstream&,int,std::string);
    void file_chosen();
    void check_password_file();
    void check_password_mask();
    void change_security();
    void add_new_mask();
    void del_mask();
    void mask_chosen();
    void timerEvent(QTimerEvent *);

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};
#endif // MAINWINDOW_H
