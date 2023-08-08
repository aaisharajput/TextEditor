#include <iostream>
#include <fstream>
#include <string>
#include <ncurses/ncurses.h>
#include <sys/types.h> 
#include <dirent.h>
#include <vector>
#include "editor.h"
using namespace std;

class PROJECT:private DATA_MEMBERS{
private:
    string filename;
    int x_value=19;
public:

    //print menu 
    void menu_list(){
        string m="[N/n] Create File\t[I/i] Insert/Edit\t[A/a] Append\t[D/d] Delete\t[S/s] Show Files\t[R/r] Read File\t   [ctrl+x] Exit";
       
        wclear(menu),wprintw(menu, m.c_str()), wrefresh(menu);
        mvwprintw(menu,2,0, "->"), wrefresh(menu);
        wmove(menu, 2, 2);
    }


    //input filename
    string getFileName() {
        wclear(disply), wprintw(disply, " "), wrefresh(disply);
       
        int ch; // for single character input
        filename="";
        x_value=19;
        mvwprintw(menu, 2, 2, "Enter filename: ");

        wmove(menu, 2, 19);
        noecho();
        wrefresh(menu);
    while(true){
        
        keypad(menu, true);
        ch = wgetch(menu);
        wclear(msg), wprintw(msg,"Max filename length: 20 char"), wrefresh(msg);
        switch(ch){
            // newline
            case KEY_ENTER:
            case '\n' : return filename;
            
            // backspace
            case KEY_BACKSPACE:
            case '\b' : x_value--;
                        if(x_value==18){  //goto last char of upper line 
                          x_value=19;
                        }

                        mvwdelch(menu, 2, x_value); // remove from screen

                        if(filename!="") //while string is not empty,delete the char
                          filename.pop_back(); 
                        break;
            //sapce key
            case ' ': wmove(menu,2,x_value);
                      break;

            //arrow keys
            case KEY_UP: wmove(menu,2,x_value);
                         break;
            case KEY_DOWN: wmove(menu,2,x_value);
                           break;
            case KEY_RIGHT: wmove(menu,2,x_value);
                            break;
            case KEY_LEFT:  wmove(menu,2,x_value);
                            break;
                            
            // ACTUAL INPUT ON SCREEN AND STRING
            default   : x_value++;
                        if(x_value<=39){
                            waddch(menu, ch); // print on screen
                            wrefresh(menu);
                            filename.push_back(ch);  // add in string
                        }else{
                             x_value=39;
                             wmove(menu,2,x_value);
                        }
                           
                        break;
        }
    }
       

    }

    //check whether file is exist or not
    void check_file_existence(string filename){
        ifstream infile(filename);
    
        if (infile.good()) {
            wclear(msg), wprintw(msg, "File %s already exists",filename.c_str()), wrefresh(msg);
        }else{
            
            ofstream outfile(filename);
            wclear(msg), wprintw(msg, "File %s created successfully",filename.c_str()), wrefresh(msg);
            outfile.close();
        }
        infile.close();
        
    }


    //insert content in the file
    void insert_data(){
        filename=getFileName();
        ifstream infile(filename);
    
        if (infile.good()) {
            EDITOR e;
            string content=e.edit_file(filename,0);
            
            if(content!=""){
                ofstream outfile(filename);
                outfile<<content;
                outfile.close();
            }
                
            if(save==true)
                wclear(msg), wprintw(msg, "Filename: %s -> Data inserted successfully",filename.c_str()), wrefresh(msg);
        }else{
            wclear(msg), wprintw(msg, "File %s not found",filename.c_str()), wrefresh(msg);
        }
        infile.close();
         
    }


    //delete file
    void delete_file(){
        filename=getFileName();
        int status = remove(filename.c_str());
            if(status==0)
                 wclear(msg), wprintw(msg, "File %s deleted successfully",filename.c_str()), wrefresh(msg);
            else
                wclear(msg), wprintw(msg, "Error Occured!! File doesn't exist"), wrefresh(msg);
        
    }
   

    //show filenames in the current dir
    void show_file(){
        DIR *pdir; 
        struct dirent *pent;

        // Open the directory 
        pdir = opendir("./"); 

        // Check for NULL pointer 
        if (pdir == NULL) { 
            wclear(disply), wprintw(disply, "Couldn't open the directory"), wrefresh(disply);
        }else{
            // Iterate over all entries 
            dis_y=3,dis_x=2;
            wclear(disply);
            while ((pent = readdir(pdir)) != NULL) { 
                mvwprintw(disply,dis_y,dis_x,pent->d_name);
                dis_y++;
            } 
            wrefresh(disply);
        }
        // Close the directory 
        closedir(pdir); 
    }

    //display the content of the file
    int read_file(string filename){
        int lineNo=0;
        //open a file in read mode.
        ifstream readfile(filename); 

        //if file not exist
        if(!readfile.good()){
            wclear(msg), wprintw(msg, "File %s not exist!!",filename.c_str()), wrefresh(msg);
            readfile.close();
            return 0;
        }

        dis_y=4,dis_x=0;
        string content="",line;

       
        wclear(disply);
        while(!readfile.eof())
        // read the file until eof occured.
        {
            getline(readfile, line);
            content+=line;
            dis_y++;
            lineNo++;
            mvwprintw(disply,dis_y,dis_x,"[%d] %s",lineNo,line.c_str(),'\n');
        }
         wrefresh(disply);

        // close the opened file.
        readfile.close(); 
        wclear(msg), wprintw(msg, "[Read Mode]    File: %s\t\t[ctrl+f]-Find\t\t[ctrl+x]-Exit",filename.c_str()), wrefresh(msg); 
        return 1;
    }


    void findData_loop(string filename){  
        menu_list();
        int chk_file_exist=read_file(filename);
        if(chk_file_exist==0)
            return;
        int chh;
        string substring;
        bool chk=true;
        while(chk){
            keypad(menu, true);
            chh = wgetch(menu);
            switch(chh){
                case ('x' & 0x1f):
                                    wclear(disply),wrefresh(disply);
                                    chk=false; 
                                    break;
                case ('f' & 0x1f):
                                    read_file(filename);
                                    substring=substring_data(filename);
                                    search_data(substring,filename);
                                    break;
            }
        }
         
    }


    //append data
    void append_data(string filename){
        //open a file in read mode.
        ifstream readfile(filename); 

        //if file not exist
        if(!readfile.good()){
            wclear(msg), wprintw(msg, "File %s not exist!!",filename.c_str()), wrefresh(msg);
            readfile.close();
            return ;
        }

        EDITOR e;
        string app_txt=e.edit_file(filename,1);
        ofstream outfile(filename,ios::app);
        
        if(app_txt!="") // if it is not return the empty string
        {
            outfile<<"\n";
            outfile<<app_txt;
        }    
        readfile.close();
        outfile.close();
        if(save==true)
            wclear(msg), wprintw(msg, "File %s appended successfully",filename.c_str()), wrefresh(msg);
    }

    //enter substring to find
    string substring_data(string filename){ 
        string substring; 

        // Get substring 
        int ch; // for single character input
        substring="";
        x_value=22;
        menu_list();
        mvwprintw(menu, 2, 2, "Enter substring: ");

        wmove(menu, 2, 22);
        noecho();
        wrefresh(menu);
    while(true){
        
        keypad(menu, true);
        ch = wgetch(menu);
        switch(ch){
            // newline
            case KEY_ENTER:
            case '\n' : return substring;
            
            // backspace
            case KEY_BACKSPACE:
            case '\b' : x_value--;
                        if(x_value==21){  //goto last char of upper line 
                          x_value=22;
                        }

                        mvwdelch(menu, 2, x_value); // remove from screen

                        if(substring!="") //while string is not empty,delete the char
                          substring.pop_back(); 
                        break;

            //arrow keys
            case KEY_UP: wmove(menu,2,x_value);
                         break;
            case KEY_DOWN: wmove(menu,2,x_value);
                           break;
            case KEY_RIGHT: wmove(menu,2,x_value);
                            break;
            case KEY_LEFT:  wmove(menu,2,x_value);
                            break;
                            
            // ACTUAL INPUT ON SCREEN AND STRING
            default   : x_value++;
                        if(x_value<=50){
                            waddch(menu, ch); // print on screen
                            wrefresh(menu);
                            substring.push_back(ch);  // add in string
                        }else{
                             x_value=50;
                             wmove(menu,2,x_value);
                        }
                           
                        break;
        }
    }

    }

    //search substring
    void search_data(string substring,string filename){
        string line; 
        int lineNumber = 0; 
        int wordCount = 0;
        x_value=15;
         // Open the file 
        ifstream infile; 
        infile.open(filename);

        // Check if the file is open 
        if(infile.is_open()) { 
            mvwprintw(disply,1,2,"Found at line: ");
            while (getline(infile, line)) {
                lineNumber++; 
                size_t found = line.find(substring); 

                if (found != string::npos) 
                {   //npos as no-position. As a return value,indicate that no matches were found in the string
                     wordCount++; 
                     x_value+=4;
                     mvwprintw(disply,1,x_value,"%d ",lineNumber);
                }
            }
            mvwprintw(disply,3,2,"occurences of substring: %d",wordCount);
        } 
        else 
            mvwprintw(disply,2,2,"Error opening file");
        wrefresh(disply);
        // Close the file 
        infile.close();
    }

};



int main() {
    initscr();
    cbreak();    
    refresh();
    noecho();

    PROJECT p;
    char temp;
    string filename;

    while(true){
        p.menu_list();
        temp=getch();
        switch (temp)
        {
            case 'N':
            case 'n':   filename = p.getFileName();
                        p.check_file_existence(filename);
                        break;

            case 'I':
            case 'i':   p.insert_data();
                        break;
            
            case 'D':
            case 'd':   p.delete_file();
                        break;

            case 'S':
            case 's':   p.show_file();
                        break;

            case 'R':
            case 'r':   filename = p.getFileName();
                        p.findData_loop(filename);
                        break;
            
            case 'A':
            case 'a':
                        filename = p.getFileName();
                        p.append_data(filename);
                        break;

            case ('x' & 0x1f) :
                        return 0;
            default:
                        break;
        }
    }

    getch();
    endwin();
    return 0;
}

