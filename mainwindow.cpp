#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
   dir_pth_textedit = new QTextEdit(this);
   dir_pth_textedit->setGeometry(10,50,270,50);

   enter_path = new QPushButton("Enter directory", this);
   enter_path->setGeometry(285,50,100,50);
   connect(enter_path, &QPushButton::clicked, this, &MainWindow::show_dir_content);

   dir_content_list = new QListWidget(this);
   dir_content_list->setGeometry(10,120,200,140);
   connect(dir_content_list,&QListWidget::itemClicked,this,&MainWindow::file_chosen);


   dir_masks = new QListWidget(this);
   dir_masks->setGeometry(10,280,200,140);
   connect(dir_masks,&QListWidget::itemClicked,this,&MainWindow::mask_chosen);

   add_mask = new QPushButton("Add",this);
   add_mask->setGeometry(240,370,60,50);
   add_mask->setDisabled(true);
   add_mask->hide();
   connect(add_mask,&QPushButton::clicked,this,&MainWindow::add_new_mask);


   delete_mask = new QPushButton("Delete",this);
   delete_mask->setGeometry(310,370,60,50);
   delete_mask->setDisabled(true);
   delete_mask->hide();
   connect(delete_mask,&QPushButton::clicked,this,&MainWindow::del_mask);


   security_change_btn = new QPushButton(this);
   security_change_btn->setGeometry(265,210,80,50);
   security_change_btn->setDisabled(true);
   security_change_btn->hide();
   connect(security_change_btn,&QPushButton::clicked,this,&MainWindow::change_security);


   password_linedit_file = new QLineEdit(this);
   password_linedit_file->setGeometry(240,175,130,30);
   password_linedit_file->setPlaceholderText("password:");
   password_linedit_file->hide();
   connect(password_linedit_file,&QLineEdit::textChanged,this,&MainWindow::check_password_file);


   password_linedit_mask = new QLineEdit(this);
   password_linedit_mask->setGeometry(240,335,130,30);
   password_linedit_mask->setPlaceholderText("password:");
   password_linedit_mask->hide();
   connect(password_linedit_mask,&QLineEdit::textChanged,this,&MainWindow::check_password_mask);


   QTextStream out(stdout);


   std::fstream iofile("template.txt", std::ios::in | std::ios::out);
   if (!iofile)
   {
           // То выводим сообщение об ошибке и выполняем exit()
           std::cerr << "Uh oh, template.txt could not be opened!";
           exit(1);
   }

   std::getline(iofile, file_content);
iofile.close();
timer_delay = 1000;
timer = startTimer(timer_delay);
}


void MainWindow::show_dir_content(){

    QTextStream out(stdout);
    directory_files.clear();
    directory_masks.clear();
    security_change_btn->hide();
//QString c_text = QInputDialog::getText(this, "Item", "Enter new item");

    password_linedit_file->hide();

    password_linedit_mask->show();
    password_linedit_mask->setText("");
    add_mask->show();
    delete_mask->hide();

    path_to_directory = dir_pth_textedit->toPlainText().toStdString();
    int dir_pos_in_file = file_content.find('{' + path_to_directory+'}');

    is_dir_new = (dir_pos_in_file == -1);


    dir_content_list->clear();
    dir_masks->clear();

    dir = new QDir(QString::fromStdString(path_to_directory));

      if (!dir->exists())
          qWarning("The directory does not exist");

      // Метод setFilter() определяет тип файлов, которые должны быть возвращены методом entryInfoList()
      dir->setFilter(QDir::Files | QDir::AllDirs);

      // Метод setSorting() задает порядок сортировки, используемый методом entryInfoList()
      dir->setSorting(QDir::Size | QDir::Reversed);

      // Метод entryInfoList() возвращает список объектов QFileInfo для всех файлов и папок в каталоге, отфильтрованных и упорядоченных соответствующими методами
      QFileInfoList list = dir->entryInfoList();

      for (int i = 0; i < list.size(); ++i) {
             QFileInfo fileInfo = list.at(i);
             QString str = fileInfo.fileName();
             if ((str != ".") && (str != "..")) {
                 QString secured = "(nosec)";
                 if (!is_dir_new) {
                     int fl_name_in_file_pos = file_content.find('[' + str.toStdString() + ']');
                     int next_dir_pos = file_content.find('{',dir_pos_in_file+1);
                     if (fl_name_in_file_pos != -1) {
                        if((next_dir_pos == -1)||(next_dir_pos>fl_name_in_file_pos)){
                            secured = "(sec)";
                        }
                     }
                 }
                 QListWidgetItem *itm = new QListWidgetItem;
                 itm->setText(str + " " + secured);
                 if(secured == "(sec)"){
                     itm->setBackground(Qt::yellow);
                 }
                 else{
                     itm->setBackground(Qt::green);
                 }

                 directory_files.push_back(str.toStdString());
                 dir_content_list->addItem(itm);

             }
         }

         if(!is_dir_new){

             int next_dir_pos = file_content.find('{',dir_pos_in_file+1);
             std::string mask_str = "-";

             int mask_str_beg_pos = file_content.find('@', dir_pos_in_file);
             int mask_str_end_pos = file_content.find('#', dir_pos_in_file);

             if ((mask_str_beg_pos != -1)&&(mask_str_end_pos!=-1)) {
                if((next_dir_pos == -1)||(next_dir_pos>mask_str_end_pos)){
                    mask_str = file_content.substr(mask_str_beg_pos+1,mask_str_end_pos - mask_str_beg_pos -1);
                }
             }

             if(mask_str!="-"){
                 while (!mask_str.empty()) {
                     QListWidgetItem *itm = new QListWidgetItem;

                     int delimeter_pos = mask_str.find('%',0);

                     if(delimeter_pos != -1){
                         directory_masks.push_back(mask_str.substr(0,delimeter_pos));
                         itm->setText(QString::fromStdString(mask_str.substr(0,delimeter_pos)));
                         mask_str.erase(0,delimeter_pos+1);
                     }else{
                         directory_masks.push_back(mask_str);

                         itm->setText(QString::fromStdString(mask_str));
                         mask_str = "";
                     }

                     dir_masks->addItem(itm);



                 }



             }else{
                 out << 2222222;
             }
         }

}

void MainWindow::file_chosen(){

    QTextStream out(stdout);
    out << 9;

    int dirpos = file_content.find('{' + path_to_directory+'}');
    int fl_name_in_file_pos = file_content.find('[' + directory_files[dir_content_list->currentRow()] + ']');
    int next_dir_pos = file_content.find('{',dirpos+1);

    bool is_file_protected = false;

    if (fl_name_in_file_pos != -1) {
       if((next_dir_pos == -1)||(next_dir_pos>fl_name_in_file_pos)){
           is_file_protected = true;
       }
    }




    QString sec_button_text;
    if(is_file_protected){
        sec_button_text = "Reduce";
    }
    else{
        sec_button_text = "Protect";
    }
    security_change_btn->setText(sec_button_text);
    security_change_btn->setDisabled(true);
    security_change_btn->show();

    password_linedit_file->setText("");
    password_linedit_file->show();
}

void MainWindow::mask_chosen(){
    QTextStream out(stdout);
    out << 8;

    delete_mask->setDisabled(true);
    delete_mask->show();

    password_linedit_mask->setText("");
    password_linedit_mask->show();
}

void MainWindow::check_password_file(){

    QString entered_pwd = password_linedit_file->text();

    QString MD5;
       QByteArray str;
       str = QCryptographicHash::hash(entered_pwd.toLatin1(),QCryptographicHash::Md5);
       MD5.append(str.toHex());



    if(file_content.find('<'+MD5.toStdString()+ '>')!=-1){
        security_change_btn->setDisabled(false);
    }else{
        security_change_btn->setDisabled(true);
    }

}

void MainWindow::check_password_mask(){
    QString entered_pwd = password_linedit_mask->text();

    QString MD5;
       QByteArray str;
       str = QCryptographicHash::hash(entered_pwd.toLatin1(),QCryptographicHash::Md5);
       MD5.append(str.toHex());



    if(file_content.find('<'+MD5.toStdString()+ '>')!=-1){
        add_mask->setDisabled(false);
        delete_mask->setDisabled(false);
    }else{
        add_mask->setDisabled(true);
        delete_mask->setDisabled(true);
    }
}

void MainWindow::change_security(){

    QTextStream out(stdout);

    std::string filename = directory_files[dir_content_list->currentRow()];

    int dirpos = file_content.find('{' + path_to_directory+'}');
    int fl_name_in_file_pos = file_content.find('[' + filename + ']');
    int next_dir_pos = file_content.find('{',dirpos+1);

    bool is_file_protected = false;

    if (fl_name_in_file_pos != -1) {
       if((next_dir_pos == -1)||(next_dir_pos>fl_name_in_file_pos)){
           is_file_protected = true;
       }
    }


    std::string full_file_name = path_to_directory + '\\' + filename;

    std::vector<int>changed_for_deny_records;

    if(is_file_protected){

        int num_beg_pos = file_content.find('(', fl_name_in_file_pos);
        int num_end_pos = file_content.find(')', fl_name_in_file_pos);

        std::string num_str = file_content.substr(num_beg_pos+1,num_end_pos - num_beg_pos -1);
        int num_of_changed_records = 0;

        while (!num_str.empty()) {
            int comma_pos = num_str.find(',',0);
            if(comma_pos != -1){
                changed_for_deny_records.push_back(QString::fromStdString(num_str.substr(0,comma_pos)).toInt());
                num_str.erase(0,comma_pos+1);
            }else{
                changed_for_deny_records.push_back(QString::fromStdString(num_str).toInt());
                num_str = "";
            }

        }




        //set new descrypor

        PACL pDACL = NULL;
        PSECURITY_DESCRIPTOR pSD = NULL;
        // Получить информацию о безопасности

        if (GetNamedSecurityInfoA(full_file_name.c_str(), SE_FILE_OBJECT,
            DACL_SECURITY_INFORMATION, NULL, NULL, &pDACL, NULL,
            &pSD) != ERROR_SUCCESS)
        {
            out << "Get security information error";
            return ;
        }

        if (pDACL == NULL)
        {
            out << "ACL list is empty";
            return ;
        }
        // получить ACL-информацию
        ACL_SIZE_INFORMATION aclInfo;
        if (!GetAclInformation(pDACL, &aclInfo, sizeof(aclInfo),
            AclSizeInformation))
        {
            out << "Can't get ACL info";
            return ;
        }
        SID_NAME_USE sid_nu;
        DWORD len = 200;

        ////////////

        //DWORD cbAcl = sizeof(ACCESS_ALLOWED_ACE) + GetLengthSid(sidValue) + sizeof(DWORD);
        DWORD cbAcl = (aclInfo.AclBytesInUse) + sizeof(DWORD);

        PACL pNewDACL = NULL;
        pNewDACL = (ACL*)LocalAlloc(LPTR, cbAcl);
        if (!pNewDACL)
        {
            out << "Can't allocate memory";
            return ;
        }
        if (!InitializeAcl(pNewDACL, cbAcl, ACL_REVISION_DS))
        {
            out << "Can't initialize new DACL";
            return ;
        }

        ////////

    // Цикл перебора всех ACL-записей

        //std::cout << aclInfo.AceCount << '\n';
        for (DWORD i = 0; i < (aclInfo.AceCount); i++)
        {
            void* ace;
            if (!GetAce(pDACL, i, &ace))
            {
                out << "Can't get ACE";
                return ;
            }

            auto it = std::find(changed_for_deny_records.begin(),changed_for_deny_records.end(),i);
            if(it==changed_for_deny_records.end()){

                if (!AddAce(pNewDACL, 2, MAXWORD, ace,
                    ((ACCESS_DENIED_ACE*)ace)->Header.AceSize))
                {
                    out << "Add ACE error" << i;
                    return ;
                }
            }
            else{
                ((ACCESS_ALLOWED_ACE*)ace)->Header.AceType = ACCESS_ALLOWED_ACE_TYPE;
                if (!AddAce(pNewDACL, 2, MAXWORD, ace,
                    ((ACCESS_DENIED_ACE*)ace)->Header.AceSize))
                {
                    out << "Add ACE error" << i;
                    return ;
                }
            }

            //out << i << '\n';
        }
        ////////

        PSECURITY_DESCRIPTOR pNewSD = PSECURITY_DESCRIPTOR(LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH));
        if (!InitializeSecurityDescriptor(pNewSD, SECURITY_DESCRIPTOR_REVISION))
        {
            out << "InitializeSecurityDescriptor error";
            return ;
        }

        ////////
        if (!SetSecurityDescriptorDacl(pNewSD, true, pNewDACL, false))
        {
            out << "SetSecurityDescriptorDacl error";
            return ;
        }

        if (!SetFileSecurityA(full_file_name.c_str(), DACL_SECURITY_INFORMATION, pNewSD))
        {
            out << "SetFileSecurity error";
            return ;
        }

        file_content.erase(fl_name_in_file_pos,num_end_pos-fl_name_in_file_pos+1);

        if(file_content.length() == (dirpos+path_to_directory.length()+2-1+1)){
            file_content.erase(dirpos,path_to_directory.length()+2);
        }else if(file_content[dirpos+path_to_directory.length()+2-1+1]=='{'){
            file_content.erase(dirpos,path_to_directory.length()+2);
        }

        std::fstream iofile("template.txt", std::ios::in | std::ios::out|std::ios::trunc);
        if (!iofile)
        {
                // То выводим сообщение об ошибке и выполняем exit()
                std::cerr << "Uh oh, template.txt could not be opened!";
                exit(1);
        }


        iofile<<file_content;
     iofile.close();

    }
    else {

        PACL pDACL = NULL;
        PSECURITY_DESCRIPTOR pSD = NULL;
        // Получить информацию о безопасности

        if (GetNamedSecurityInfoA(full_file_name.c_str(), SE_FILE_OBJECT,
            DACL_SECURITY_INFORMATION, NULL, NULL, &pDACL, NULL,
            &pSD) != ERROR_SUCCESS)
        {
            out << "Get security information error";
            return ;
        }

        if (pDACL == NULL)
        {
            out << "ACL list is empty";
            return ;
        }
        // получить ACL-информацию
        ACL_SIZE_INFORMATION aclInfo;
        if (!GetAclInformation(pDACL, &aclInfo, sizeof(aclInfo),
            AclSizeInformation))
        {
            out << "Can't get ACL info";
            return ;
        }
        SID_NAME_USE sid_nu;
        DWORD len = 200;

        ////////////

        //DWORD cbAcl = sizeof(ACCESS_ALLOWED_ACE) + GetLengthSid(sidValue) + sizeof(DWORD);
        DWORD cbAcl = (aclInfo.AclBytesInUse) + sizeof(DWORD);

        PACL pNewDACL = NULL;
        pNewDACL = (ACL*)LocalAlloc(LPTR, cbAcl);
        if (!pNewDACL)
        {
            out << "Can't allocate memory";
            return ;
        }
        if (!InitializeAcl(pNewDACL, cbAcl, ACL_REVISION_DS))
        {
            out << "Can't initialize new DACL";
            return ;
        }

        ////////

    // Цикл перебора всех ACL-записей

        //std::cout << aclInfo.AceCount << '\n';

        for (DWORD i = 0; i < aclInfo.AceCount; i++)
            {
                void* ace;
                if (!GetAce(pDACL, i, &ace))
                {
                    out<<"Can't get ACE";
                    return ;
                }



                if (((ACCESS_ALLOWED_ACE*)ace)->Header.AceType == ACCESS_ALLOWED_ACE_TYPE)
                {
                    changed_for_deny_records.push_back(i);
                    ((ACCESS_DENIED_ACE*)ace)->Header.AceType = ACCESS_DENIED_ACE_TYPE;

                    if (!AddAce(pNewDACL, 2, MAXWORD, ace,
                        ((ACCESS_DENIED_ACE*)ace)->Header.AceSize))
                    {
                        out<< "Add ACE error" << i;
                        return ;
                    }

                }

                //out << i << '\n';
            }
        ////////

        PSECURITY_DESCRIPTOR pNewSD = PSECURITY_DESCRIPTOR(LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH));
        if (!InitializeSecurityDescriptor(pNewSD, SECURITY_DESCRIPTOR_REVISION))
        {
            out << "InitializeSecurityDescriptor error";
            return ;
        }

        ////////
        if (!SetSecurityDescriptorDacl(pNewSD, true, pNewDACL, false))
        {
            out << "SetSecurityDescriptorDacl error";
            return ;
        }

        if (!SetFileSecurityA(full_file_name.c_str(), DACL_SECURITY_INFORMATION, pNewSD))
        {
            out << "SetFileSecurity error";
            return ;
        }

        QString numstr;
        for (int i=0; i<changed_for_deny_records.size(); i++) {
            QString temp_num;
            temp_num.setNum(changed_for_deny_records[i]);
            numstr.append(temp_num+',');
        }
        numstr.remove(numstr.length()-1,1);

        if(is_dir_new){
            file_content.append('{'+path_to_directory+'}'+'['+filename+']'+'('+numstr.toStdString()+')');
        }else{
            std::string str_to_past = '['+filename+']'+'('+numstr.toStdString()+')';
            file_content.insert(dirpos+path_to_directory.length()+2-1+1,str_to_past);

        }
        std::fstream iofile("template.txt", std::ios::in | std::ios::out|std::ios::trunc);
        if (!iofile)
        {
                // То выводим сообщение об ошибке и выполняем exit()
                std::cerr << "Uh oh, template.txt could not be opened!";
                exit(1);
        }


        iofile<<file_content;
     iofile.close();

    }
    show_dir_content();
}

void MainWindow::del_mask(){
    int r = dir_masks->currentRow();

      if (r != -1) {
        QListWidgetItem *item = dir_masks->takeItem(r);

        std::string deleted_name = item->text().toStdString();

        int dir_pos_in_file = file_content.find('{' + path_to_directory+'}');

        if((r == 0)&&(directory_masks.size()==1)){

            int mask_pos = file_content.find('@'+deleted_name+'#',dir_pos_in_file);
            file_content.erase(mask_pos,deleted_name.length()+2);
        }
        else if((r == 0)&&(directory_masks.size()>1)){

            int mask_pos = file_content.find('@'+deleted_name+'%',dir_pos_in_file);
            file_content.erase(mask_pos+1,deleted_name.length()+1);
        }else if(r == directory_masks.size()-1){

            int mask_pos = file_content.find('%'+deleted_name+'#',dir_pos_in_file);
            file_content.erase(mask_pos,deleted_name.length()+1);
        }else if((r > 0)&&(r<directory_masks.size()-1)){

            int mask_pos = file_content.find('%'+deleted_name+'%',dir_pos_in_file);
            file_content.erase(mask_pos,deleted_name.length()+1);
        }



        directory_masks.erase(directory_masks.begin()+r);

        if(file_content.length() == (dir_pos_in_file+path_to_directory.length()+2-1+1)){
            file_content.erase(dir_pos_in_file,path_to_directory.length()+2);
        }else if(file_content[dir_pos_in_file+path_to_directory.length()+2-1+1]=='{'){
            file_content.erase(dir_pos_in_file,path_to_directory.length()+2);
        }


        delete item;

        std::fstream iofile("template.txt", std::ios::in | std::ios::out|std::ios::trunc);
        if (!iofile)
        {
                // То выводим сообщение об ошибке и выполняем exit()
                std::cerr << "Uh oh, template.txt could not be opened!";
                exit(1);
        }

        iofile<<file_content;
     iofile.close();


    show_dir_content();
      }
}

void MainWindow::add_new_mask(){
    QTextStream out(stdout);

    QString mask_str;
    out << 10;
    QString r_text = QInputDialog::getText(this, "Mask",
          "Enter new mask", QLineEdit::Normal);
    out << 34;

    if(is_dir_new){
        file_content.append('{'+path_to_directory+'}'+'@'+r_text.toStdString()+'#');
    }else{
        int dir_pos_in_file = file_content.find('{' + path_to_directory+'}');

        if(!directory_masks.empty()){
            int mask_end_pos = file_content.find('#',dir_pos_in_file);
            std::string str_to_past = '%'+ r_text.toStdString();
            file_content.insert(mask_end_pos,str_to_past);

        }else{

            int next_dir_pos = file_content.find('{',dir_pos_in_file+1);
            std::string str_to_past = '@'+ r_text.toStdString()+'#';
               if(next_dir_pos == -1){

                   file_content.append(str_to_past);
               }else{
                   file_content.insert(next_dir_pos,str_to_past);
               }

        }

    }
    std::fstream iofile("template.txt", std::ios::in | std::ios::out|std::ios::trunc);
    if (!iofile)
    {
            // То выводим сообщение об ошибке и выполняем exit()
            std::cerr << "Uh oh, template.txt could not be opened!";
            exit(1);
    }


    iofile<<file_content;
 iofile.close();


show_dir_content();
}

void MainWindow::timerEvent(QTimerEvent *event){
    QTextStream out(stdout);
    int dir_pos_in_file = file_content.find('{');
    std::vector<std::vector<std::string>> dirs_and_noncreations;

    while(dir_pos_in_file!=-1){

        std::string check_path_to_dir = file_content.substr(dir_pos_in_file+1,file_content.find('}',dir_pos_in_file+1) - dir_pos_in_file - 1);
        dir = new QDir(QString::fromStdString(check_path_to_dir));

          if (!dir->exists())
              qWarning("The directory does not exist");

          // Метод setFilter() определяет тип файлов, которые должны быть возвращены методом entryInfoList()
          dir->setFilter(QDir::Files | QDir::AllDirs);

          // Метод setSorting() задает порядок сортировки, используемый методом entryInfoList()
          dir->setSorting(QDir::Size | QDir::Reversed);

          // Метод entryInfoList() возвращает список объектов QFileInfo для всех файлов и папок в каталоге, отфильтрованных и упорядоченных соответствующими методами
          QFileInfoList list = dir->entryInfoList();

          for (int i = 0; i < list.size(); ++i) {
                 QFileInfo fileInfo = list.at(i);
                 QString str = fileInfo.fileName();
                 if ((str != ".") && (str != "..")) {

                     int next_dir_pos = file_content.find('{',dir_pos_in_file+1);
                     std::string mask_str = "-";

                     int mask_str_beg_pos = file_content.find('@', dir_pos_in_file);
                     int mask_str_end_pos = file_content.find('#', dir_pos_in_file);

                     if ((mask_str_beg_pos != -1)&&(mask_str_end_pos!=-1)) {
                        if((next_dir_pos == -1)||(next_dir_pos>mask_str_end_pos)){
                            mask_str = file_content.substr(mask_str_beg_pos+1,mask_str_end_pos - mask_str_beg_pos -1);
                        }
                     }

                     if(mask_str!="-"){
                         out << QString::fromStdString( mask_str);

                         bool delete_file = false;
                         while ((!mask_str.empty())&&(!delete_file)) {

                             int delimeter_pos = mask_str.find('%',0);



                             if(delimeter_pos != -1){
                                 std::string temp_mask = mask_str.substr(0,delimeter_pos);
                                 delete_file = true;
                                 if(str.toStdString().length() == temp_mask.length()){
                                     for (int k = 0; k < temp_mask.length(); k++) {
                                         if((str.toStdString()[k] != temp_mask[k])&&(temp_mask[k]!='*')){
                                             delete_file = false;
                                             break;
                                         }
                                     }
                                 }else{
                                     delete_file = false;
                                 }

                                 mask_str.erase(0,delimeter_pos+1);



                             }else{
                                 std::string temp_mask = mask_str;
                                 delete_file = true;
                                 if(str.toStdString().length() == temp_mask.length()){
                                     for (int k = 0; k < temp_mask.length(); k++) {
                                         if((str.toStdString()[k] != temp_mask[k])&&(temp_mask[k]!='*')){
                                             delete_file = false;
                                             out << k << '\n';
                                             break;
                                         }
                                     }
                                 }
                                 else{
                                       delete_file = false;
                                                                  }
                                 mask_str = "";


                             }


                             if(mask_str.empty()){
                                out << 6;
                             }




                         }
                         if(delete_file){
                             QFile::remove(QString::fromStdString(check_path_to_dir) + '\\' + str);

                         }


                     }

                 }
             }





        dir_pos_in_file = file_content.find('{',dir_pos_in_file+1);
        if(dir_pos_in_file == -1){
           out << 0;
        }
    }



}

MainWindow::~MainWindow()
{

}


















