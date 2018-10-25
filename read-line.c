/*
 * CS252: Systems Programming
 * Purdue University
 * Example that shows how to read one line with simple editing
 * using raw terminal.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUFFER_LINE 2048

extern void tty_raw_mode(void);

// Buffer where line is stored
int line_length;
char line_buffer[MAX_BUFFER_LINE];

// Simple history array
// This history does not change. 
// Yours have to be updated.
int history_index = 0;
char * history [] = {
  ""
};
int history_length = sizeof(history)/sizeof(char *);

void read_line_print_usage()
{
  char * usage = "\n"
    " ctrl-?       Print usage\n"
    " Backspace    Deletes last character\n"
    " up arrow     See last command in the history\n";

  write(1, usage, strlen(usage));
}

//flag for first item in history
int histArrowFlag = -1;
int startHistFlag = 0;

char *temp_line;

/* 
 * Input a line with some basic editing.
 */
char * read_line() {

  // Set terminal in raw mode
  tty_raw_mode();

  line_length = 0;

	//used for current cursor location
	int line_loc = line_length;


  // Read one line until enter is typed
  while (1) {

    // Read one character in raw mode.
    char ch;
    read(0, &ch, 1);

    if (ch>=32) {
      // It is a printable character. 

			//backspace
			if (ch == 127) {
				if (line_length > 0) {

					if (line_length == line_loc) {

						// <backspace> was typed. Remove previous character read.

						// Go back one character
						ch = 8;
						write(1,&ch,1);

						// Write a space to erase the last character read
						ch = ' ';
						write(1,&ch,1);

						// Go back one character
						ch = 8;
						write(1,&ch,1);

					} else {
						// Print backspaces
						ch = 8;
						write(1, &ch,1);

						//shifts characters back one
						for (int k = line_loc; k < line_length; k++) {
							write(1, &(line_buffer[k]), 1);
						}
	
						//prints space
						char space = ' ';
						write(1, &space, 1);

						// Print backspaces
						int i = 0;
						for (i =0; i < line_length - line_loc + 1; i++) {
							ch = 8;
							write(1,&ch,1);
						}
	
						//updates line buffer
						for (int k = line_loc - 1; k < line_length - 1; k++) {
							line_buffer[k] = line_buffer[k+1];
						}
						
							

					}

					// Remove one character from buffer
					line_length--;
					line_loc--;
				}
				continue;
			
			}


      // Do echo
			if (line_loc == line_length) {
      	write(1,&ch,1);
			} else {
				write(1,&ch,1);
				for (int k = line_loc; k < line_length; k++) {
					write(1, &(line_buffer[k]), 1);
				}

				// Print backspaces
				int i = 0;
				for (i =0; i < line_length - line_loc; i++) {
					int ccc = 8;
					write(1,&ccc,1);
				}
			}

      // If max number of character reached return.
      if (line_length==MAX_BUFFER_LINE-2) break; 

      // add char to buffer.
			if (line_loc == line_length) {
      	line_buffer[line_length]=ch;
			} else {
				for (int k = line_length; k >= line_loc; k--) {
					line_buffer[k + 1] = line_buffer[k];
				}
				line_buffer[line_loc]=ch;
			}
     	line_length++;
			line_loc++;
			
		
    }
    else if (ch==10) {
      // <Enter> was typed. Return line
      
      // Print newline
      write(1,&ch,1);

      break;
    }
    else if (ch == 31) {
      // ctrl-?
      read_line_print_usage();
      line_buffer[0]=0;
      break;
    }
    else if (ch == 8) {	
			if (line_length > 0) {

				if (line_length == line_loc) {

					// <backspace> was typed. Remove previous character read.

					// Go back one character
					ch = 8;
					write(1,&ch,1);

					// Write a space to erase the last character read
					ch = ' ';
					write(1,&ch,1);

					// Go back one character
					ch = 8;
					write(1,&ch,1);

				} else {
					// Print backspaces
					ch = 8;
					write(1, &ch,1);

					//shifts characters back one
					for (int k = line_loc; k < line_length; k++) {
						write(1, &(line_buffer[k]), 1);
					}

					//prints space
					char space = ' ';
					write(1, &space, 1);

					// Print backspaces
					int i = 0;
					for (i =0; i < line_length - line_loc + 1; i++) {
						ch = 8;
						write(1,&ch,1);
					}

					//updates line buffer
					for (int k = line_loc - 1; k < line_length - 1; k++) {
						line_buffer[k] = line_buffer[k+1];
					}
					
						

				}

				// Remove one character from buffer
				line_length--;
				line_loc--;
			}

    }
    else if (ch==27) {
      // Escape sequence. Read two chars more
      //
      // HINT: Use the program "keyboard-example" to
      // see the ascii code for the different chars typed.
      //
      char ch1; 
      char ch2;
      read(0, &ch1, 1);
      read(0, &ch2, 1);
      if (ch1==91 && ch2==65 && startHistFlag) {
				// Up arrow. Print next line in history.

				//stores value in temp
				if (history_index == history_length - 1) {
					temp_line = (char *) malloc(line_length * sizeof(char) + 1);
					strcpy(temp_line, line_buffer);
					temp_line[line_length] = '\0';
				}


				//printf("--------%d--------%d--------\n", history_length, history_index);
				// Erase old line
				// Print backspaces
				int i = 0;
				for (i =0; i < line_length; i++) {
					ch = 8;
					write(1,&ch,1);
				}

				// Print spaces on top
				for (i =0; i < line_length; i++) {
					ch = ' ';
					write(1,&ch,1);
				}

				// Print backspaces
				for (i =0; i < line_length; i++) {
					ch = 8;
					write(1,&ch,1);
				}	
	
				//checks if last arrow was down arrow	
				if (histArrowFlag == 0) {
					history_index--;
				}	
			
	
				// Copy line from history
				if (history_index != 0) {
					strcpy(line_buffer, history[history_index]);
					line_length = strlen(line_buffer);
					line_loc = line_length;
					if (history_index > 1) {
						history_index--;
					}
					histArrowFlag = 1;
				
					//history_index=(history_index-1)%history_length;
				}
				
				// echo line
				write(1, line_buffer, line_length);
      } else if (ch1==91 && ch2==66 && startHistFlag) {
				//Down arrow. Print previous line in history.
				

				// Erase old line
				// Print backspaces
				int i = 0;
				for (i =0; i < line_length; i++) {
					ch = 8;
					write(1,&ch,1);
				}

				// Print spaces on top
				for (i =0; i < line_length; i++) {
					ch = ' ';
					write(1,&ch,1);
				}

				// Print backspaces
				for (i =0; i < line_length; i++) {
					ch = 8;
					write(1,&ch,1);
				}	

				// Copy line from history
			
				//printf("b--------%d--------%d--------\n", history_length, history_index);

				//checks if last arrow was up arrow
				if (histArrowFlag == 1 && history_index != 0) {
					history_index++;
				}			
	
				if (history_length > 1 && history_index < history_length - 1) {
					//down arrow when in history
					strcpy(line_buffer, history[++history_index]);
					line_length = strlen(line_buffer);
					line_loc = line_length;

					histArrowFlag = 0;
					//history_index++;	
					//history_index=(history_index+1)%history_length;
				} else if (history_index >= history_length - 1) {
					//down arrow for going to temp 					
					strcpy(line_buffer, temp_line);
					

					line_length = strlen(temp_line);
					line_loc = line_length;
					

					histArrowFlag = -1;
				}
				// echo line
				write(1, line_buffer, line_length);

				//printf("a--------%d--------%d--------\n", history_length, history_index);
			} else if (ch1==91 && ch2==68) {
				//left arrow
				if (line_loc > 0) {
					ch = 27;
					write(1, &ch, 1);
					ch = 91;
					write(1, &ch, 1);
					ch = 68;
					write(1, &ch, 1);
					line_loc--;
				}

			} else if (ch1==91 && ch2==67) {
				//right arrow
				if (line_loc < line_length) {
					ch = 27;
					write(1, &ch, 1);
					ch = 91;
					write(1, &ch, 1);
					ch = 67;
					write(1, &ch, 1);
					line_loc++;
				}

			} else if (ch1==91 && ch2==72) {
				//home
				while (line_loc > 0) {
					ch = 27;
					write(1, &ch, 1);
					ch = 91;
					write(1, &ch, 1);
					ch = 68;
					write(1, &ch, 1);
					line_loc--;
				}
			} else if (ch1==91 && ch2==70) {
				//end
				while (line_loc != line_length) {
					ch = 27;
					write(1, &ch, 1);
					ch = 91;
					write(1, &ch, 1);
					ch = 67;
					write(1, &ch, 1);
					line_loc++;
				}

			} else if (ch1==91 && ch2==51) {
				//delete
				if (line_length > 0 && line_loc != line_length) {

					//shift character back one
					for (int k = line_loc + 1; k < line_length; k++) {
						write(1, &(line_buffer[k]), 1);
					}

					//print space
					char space = ' ';
					write(1, &space, 1);

					// Print backspaces
					int i = 0;
					for (i =0; i < line_length - line_loc; i++) {
						ch = 8;
						write(1,&ch,1);
					}
				
					//update buffer
					for (int k = line_loc; k < line_length - 1; k++) {
						line_buffer[k] = line_buffer[k+1];
					}
					
					// Remove one character from buffer
					line_length--;

				}

			}
			//printf("%d.%d\n", ch1, ch2);
      
    } else if (ch == 1) {
			//home
			while (line_loc > 0) {
				ch = 27;
				write(1, &ch, 1);
				ch = 91;
				write(1, &ch, 1);
				ch = 68;
				write(1, &ch, 1);
				line_loc--;
			}

		} else if (ch == 5) {
			//end
			while (line_loc != line_length) {
				ch = 27;
				write(1, &ch, 1);
				ch = 91;
				write(1, &ch, 1);
				ch = 67;
				write(1, &ch, 1);
				line_loc++;
			}


		} else if (ch == 4) {
			//delete
			if (line_length > 0 && line_loc != line_length) {

				//shift character back one
				for (int k = line_loc + 1; k < line_length; k++) {
					write(1, &(line_buffer[k]), 1);
				}

				//print space
				char space = ' ';
				write(1, &space, 1);

				// Print backspaces
				int i = 0;
				for (i =0; i < line_length - line_loc; i++) {
					ch = 8;
					write(1,&ch,1);
				}
			
				//update buffer
				for (int k = line_loc; k < line_length - 1; k++) {
					line_buffer[k] = line_buffer[k+1];
				}
				line_loc--;

				// Print backspaces
				ch = 8;
				write(1, &ch,1);

				//shifts characters back one
				for (int k = line_loc; k < line_length; k++) {
					write(1, &(line_buffer[k]), 1);
				}

				//prints space
				write(1, &space, 1);

				// Print backspaces
				for (i =0; i < line_length - line_loc + 1; i++) {
					ch = 8;
					write(1,&ch,1);
				}

				//updates line buffer
				for (int k = line_loc - 1; k < line_length - 1; k++) {
					line_buffer[k] = line_buffer[k+1];
				}




				
				// Remove one character from buffer
				line_length--;

			}


		}

  }

  // Add eol and null char at the end of string
  line_buffer[line_length]=10;
  line_length++;
  line_buffer[line_length]=0;


	//checks for empty line
	if (strcmp(line_buffer, "") != 10 && strcmp(line_buffer, "") != 32) {

		//update history
		history[history_length] = (char *) malloc(strlen(line_buffer) * sizeof(char) + 1);
		strcpy(history[history_length++], line_buffer);

		history[history_length - 1][strlen(line_buffer) - 1] = '\0';
		startHistFlag = 1;

	}

	history_index = history_length - 1;	
	
	histArrowFlag = -1;

	/*
	for(int i = 0; i < history_length; i++) {
		printf("%s\n", history[i]);
	}
	*/

	//printf("--------%d--------%d--------\n", history_length, history_index);
	

	temp_line = (char *) malloc(sizeof("") * sizeof(char) + 1);
	strcpy(temp_line, "");	

  return line_buffer;
}

