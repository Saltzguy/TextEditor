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
// Wait for one keypress and return it.
char editor_read_key(){
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1){
        if(nread == -1 && errno != EAGAIN){
            die("read");
        }
    }
    return c;
}

void editor_process_keypress(){
    char c = editor_read_key();

    switch (c) {
        case CTRL_KEY('q'):
            exit(0);
            break;
        default:
            printf("%c\r\n", c);
            break;
    }
}

//init
int main(){

    enable_raw_mode();

    while(1){
       editor_process_keypress();
    }

    return 0;
}