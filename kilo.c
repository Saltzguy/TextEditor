#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct termios orig_termios;

void disable_raw_mode(){
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}
// Disable ECHO in the terminal, 
// sets orgin_Termios to the user termimal settings,
// and registers disable_raw_mode as the exit function
void enable_raw_mode(){
    tcgetattr(STDIN_FILENO, &orig_termios);
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

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}


int main(){

    enable_raw_mode();

    while(1){
        char c ='\0';

        read(STDIN_FILENO, &c, 1);
        //Test if c is a control character
        if(iscntrl(c)){
            printf("%d\r\n",c);
        }
        else{
            printf("%d ('%c')\r\n", c, c);
        }
        if(c == 'q'){
            break;
        }
    }

    return 0;
}