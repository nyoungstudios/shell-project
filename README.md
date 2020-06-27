# My Shell Project
My shell project that I created for the Systems Programming class at Purdue. Got over 100% on this project.

## Features
1. Features specified in the handout that work.  
Everything works including:  
Parsing and executing commands  
File redirection >, >>, <, >&, and >>&  
Pipes |  
isatty()  
Ctrl+C  
Zombie Elimination  
Exit the shell  
Quotes "" and ''  
Escaping special characters \\  
Builtin Functions (printenv, setenv, unsetenv, source, cd)  
Source .shellrc file  
Source with environment variables  
Subshells $()  
Process substitution  
Environment variable expansion ${}  
Tilde expansion (~)  
Wildcarding (*, ?)  
Edit mode (arrow keys, Ctrl+D for delete, Ctrl+H for backspace, Ctrl+A for home, Ctrl+E for end)  
History up and down arrow keys  
Editing history commands  
Variable prompt  

2. Features specified in the handout that do not work.  
Nothing

3. Extra features I have implemented.  
cd - . This returns to the last directory the user was in before the current directory. Also updates PWD and OLDPWD environment variables for all cd commands.  
$?, $_, $SHELL, $anything. Extra environment variable parsing without the curly brackets like zsh.  
Error statement if command not found

