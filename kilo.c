// Includes
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

//Defines
#define CTRL_KEY(k) ((k) & 0x1f)


//Data
struct termios orig_termios;

//Terminal settings
void die(const char *s){
    perror(s);
    exit(1);
}
void disable_raw_mode(){
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1){
        die("tcsetattr");
    }
}
// Disable ECHO in the terminal, 
// sets orgin_Termios to the user termimal settings,
// and registers disable_raw_mode as the exit function
void enable_raw_mode(){
    if (tcgetattr(STDIN_FILENO, &orig_termios)== -1){
        die("tcgetattr");
    }
    atexit(disable_raw_mode);


    struct termios raw = orig_termios;
    //Disable Ctrl S and Ctrl Q
    //Disable ctrl m
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    // Turns off all out processing
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    //ICANON turns off canonical mode.
    //ISIG turns of Ctrl C and Ctrl Z 
    //IEXTEN disable ctrl v
    //Program will quit as soon as the user hits q.
    raw.c_lflag &= ~(ECHO | ICANON |IEXTEN| ISIG);

    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1){
        die("tcsetattr");
    }
}

//init
int main(){

    enable_raw_mode();

    while(1){
        char c ='\0';

        if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN){
            die("read");
        }
        //Test if c is a control character
        if(iscntrl(c)){
            printf("%d\r\n",c);
        }
        else{
            printf("%d ('%c')\r\n", c, c);
        }
        if(c == CTRL_KEY('q')){
            break;
        }
    }

    return 0;
}