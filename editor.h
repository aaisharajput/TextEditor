#include<iostream>
#include<string>
#include<fstream>
#include<ncurses/ncurses.h>
using namespace std;
//right ka \n, redo,undo  
//cd mca\sem2\program\project
//g++ TextEditor.cpp -lncurses
// .\a.exe
//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
class DATA_MEMBERS{
  protected: 
    int maxX,maxY ;
    int curr_y, curr_x;
    int dis_x,dis_y;
    bool save,chk;
    string redo,undo="";
    WINDOW *dwin,*msg,*menu,*disply;
    
    int ch,line_count; // ch for single character input
    string text; // actual data string
    char temp;
  public:
    DATA_MEMBERS(){
     maxX = getmaxx(stdscr)-2; // max cols
     maxY = getmaxy(stdscr)-2; // max rows
     curr_y=2, curr_x=5;
     dwin= newwin(maxY,maxX, 2, 2);
     msg=newwin(1, maxX, maxY-1, 2);
     menu=newwin(3, maxX, 0, 2);
     disply=newwin(maxY-3, maxX, 3, 2);
     dis_x=2;
     dis_y=3;
     line_count=3;
     save=false;
  }
};

class EDITOR:private DATA_MEMBERS{
public:
  string edit_file(string filename,int status){
    // start_color();
    // init_pair(1,COLOR_BLACK,COLOR_BLACK);
    noecho();
    wclear(menu), mvwprintw(menu,1,0, "Filename: %s",filename.c_str()), wrefresh(menu);
    wclear(msg), wprintw(msg, "[Write Mode]      ctrl+x[Exit]\t\tctrl+s[Save]"), wrefresh(msg);
    text="";
    string line;
    if(status==0){
    //displat the content of file
    ifstream readfile(filename);
    string dwin_txt;
    char end='~';
        wclear(dwin);
        while(!readfile.eof())
        // read the file until eof occured.
        {
            getline(readfile, line);
            dwin_txt=line + '~';
            line+='\n';
            text+=line;
            mvwprintw(dwin,curr_y,0,"[%d]",(line_count-2)); //print line number
            mvwprintw(dwin,curr_y,curr_x,dwin_txt.c_str(),end);
            curr_y++,line_count++;
        }
          line=text;
        if(curr_y==3 && dwin_txt=="~"){  //if file is empty
          curr_y--,line_count--,text="",line="";
          mvwprintw(dwin,curr_y,curr_x," ");
        }
            
    }

        mvwprintw(dwin,curr_y,0,"[%d]",(line_count-2)); //print line number
        wrefresh(dwin);
        wmove(dwin, curr_y, curr_x);

    while(true){
          
        keypad(dwin, true);
        ch = wgetch(dwin);
        wclear(msg), wprintw(msg, "ctrl+x[Exit]\t\tctrl+s[Save]"), wrefresh(msg);
        switch(ch){
            // ctrl + x (to exit)
            case ('x' & 0x1f) : if(save==false){
                                  wclear(msg), wprintw(msg, "Do you want to save changes(Yes-Y|y  No-any key)"), wrefresh(msg);
                                  char changes;
                                  changes=wgetch(dwin);
                                  if(changes=='Y' || changes=='y')
                                    line=text;
                                }
                                wclear(msg), wprintw(msg, "EXIT"), wrefresh(msg);
                                wmove(dwin, curr_y, curr_x);
                                getchar(); // pause screen (single char input)
                                wclear(dwin), wprintw(dwin, " "), wrefresh(dwin);
                                refresh();
                                return line;

            // ctrl + o (to save)
            case ('s' & 0x1f) : // copy var text to new node or same node;
                                wclear(msg), wprintw(msg, "DATA SAVED"), wrefresh(msg);
                                save=true;
                                line=text;
                                wmove(dwin, curr_y, curr_x);
                                break;

            // ctrl + z (to undo)
            case ('z' & 0x1f) :
                                
                                break;
            // ctrl + o (to redo)
            case ('y' & 0x1f) :

            // newline
            case KEY_ENTER:
            case '\n' : save=false;
                         waddch(dwin, '~');
                         wrefresh(dwin);
              
                        curr_x=5, curr_y++; 
                        line_count++;
                        mvwprintw(dwin,curr_y,0,"[%d]",(line_count-2)); //print line number
                        wmove(dwin, curr_y, curr_x); // bring curser to next line
                        text.push_back(ch);
                        break;
            
            // backspace
            case KEY_BACKSPACE:
            case '\b' : save=false;
                        curr_x--;
                        if(curr_x==4){  //goto last char of upper line 
                          mvwprintw(dwin,curr_y,0,"    "); //remove line number
                          curr_x=maxX-1;
                          curr_y--;

                          //move left untill char occur
                          temp=winch(dwin);
                          while((temp==' '||temp=='~')&&curr_x>5){
                            if(temp=='~'){
                              mvwdelch(dwin, curr_y, curr_x);
                              break;
                            }
                            wmove(dwin,curr_y,--curr_x);
                            temp=winch(dwin);
                          }
                       
                          line_count--;
                          
                        }
                        //if it is last line
                        if(curr_y==1){
                          curr_x=5;
                          curr_y=2;
                          line_count=2;
                          mvwprintw(dwin,curr_y,0,"[%d]",(line_count-2)); //print line number
                        }
                        
                        mvwdelch(dwin, curr_y, curr_x); // remove from screen
                        if(text!="") //while string is not empty,delete the char
                          text.pop_back(); 
                        break;

            // arrow key up
           case KEY_UP:
                        curr_y--;
                        if(curr_y < 2)
                           curr_y=2;
                        wmove(dwin, curr_y, curr_x);

                        //if space present move left until char occured
                        temp=winch(dwin);
                        if(temp==' ' || temp=='~'){
                        while((temp==' '||temp=='~')&&curr_x>5){
                            wmove(dwin,curr_y,--curr_x);
                            temp=winch(dwin);
                          }
                        
                         curr_x++;
                        wmove(dwin, curr_y, curr_x);
                        }
                        break;

            //arrow key down
           case KEY_DOWN:
                        curr_y++;
                        if(curr_y >=line_count)
                           curr_y=line_count;
                        
                        wmove(dwin, curr_y, curr_x);
                         
                        //if space present move left until char occured
                        temp=winch(dwin);
                        if(temp==' ' || temp=='~'){
                        while((temp==' '||temp=='~')&&curr_x>5){
                            wmove(dwin,curr_y,--curr_x);
                            temp=winch(dwin);
                          }
                    
                         curr_x++;
                        wmove(dwin, curr_y, curr_x);
                        }
                        break;

            // arrow key left
            case KEY_LEFT:
                          curr_x--;
                       
                          if(curr_x==4){  //goto last char of upper line 
                          curr_x=maxX-1;
                          curr_y--;

                          //move left untill char occur
                          temp=' ';
                          while((temp==' '||temp=='~')&&curr_x>5){
                            wmove(dwin,curr_y,--curr_x);
                            temp=winch(dwin);
                          }                                                           
                          curr_x++;
                        }
                        //if it is last line
                        if(curr_y==1){
                          curr_x=5;
                          curr_y=2;
                        }
                            
                          wmove(dwin, curr_y, curr_x);
                          break;

            //arrow key right
           case KEY_RIGHT:
                        
                        //move right untill enter or maxX reached
                           temp=winch(dwin);
                          if(temp=='~'|| curr_x==maxX-1){
                            curr_x=4;
                            curr_y++;
                          }
                          
                          curr_x++;

                          if(curr_y>line_count){
                            --curr_y;
                            curr_x=maxX-1;
                          }
                         wmove(dwin,curr_y,curr_x);
                        break;
            

            // ACTUAL INPUT ON SCREEN AND STRING
            default   : save=false;
                        curr_x++;
                        if(curr_x==maxX){
                          curr_x=5;
                          curr_y++;
                          line_count++;
                          mvwprintw(dwin,curr_y,0,"[%d]",(line_count-2)); //print line number
                          wmove(dwin,curr_y,curr_x);
                          curr_x++;
                        }
                        waddch(dwin, ch); // print on screen
                        wrefresh(dwin);
                        text.push_back(ch);  // add in string
                          

        }
    }

      }
};

// int main(){
//     initscr();
//     cbreak();    
//     refresh();
  
//     EDITOR e;
//     e.edit_file();

//     endwin();
//     return 0;
// }