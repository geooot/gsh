/**
 * @file	main.cpp
 * @author	George Thayamkery
 * @date	03/23/2020
 * @project	3
 * @section	501
 * @email	geooot@tamu.edu
 * 
 * @breif	gsh - George Shell
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include "functions.h"

using namespace std;

vector<pid_t> backgroundProcesses;

void runPwd() {
    pid_t pid = fork();
    if (pid == 0) {
        execlp("pwd", "pwd", NULL);
    } else {
        waitpid(pid, NULL, 0);
    }
}

/**
 * @breif	Main loop for gsh
 * 
 * @return	int	return code
 */
int main()
{
    cout << "Welcome to gsh (George Shell)!" << endl;
    while (true)
    {
        cout << endl;
        runPwd();

        for (int x=0; x<backgroundProcesses.size(); x++) {
            pid_t ret = waitpid(backgroundProcesses[x], 0, WNOHANG);
            if (ret == backgroundProcesses[x]) {
                backgroundProcesses.erase(backgroundProcesses.begin() + x);
            }
        }

        cout << "gsh$ "; //print a prompt 
        string inputline;
        getline(cin, inputline); //get a line from standard input

        bool isBackgroundLine = shouldLineBeDoneInBackground(inputline);
        if (isBackgroundLine) {
            inputline = inputline.substr(0, inputline.size() - 1);
        }

        vector<Token> tokens = parseLine(inputline);
        vector<Command> commands = tokensToCommands(tokens);

        if (commands.size() <= 0) {
            continue;
        }


        int lastReadFd = -1; // this is needed so we don't absolutely wreck the original stdin. 

        for (int x=0; x<commands.size(); x++) {
            string fileToExecute = commands[x].fileToExecute;
            vector<string> args = commands[x].args;

            // handle built in functions (cd, jobs, exit)
            if (fileToExecute == "cd") {
                string dir = "";
                if (args.size() > 0)
                    dir = args[0];
                if (dir == "-")
                    dir = "../";
                chdir(dir.c_str());
                continue;
            } else if (fileToExecute == "jobs") {
                for (int i=0; i<backgroundProcesses.size(); i++) {
                    std::cout << "[" << (i+1) << "] " << backgroundProcesses[i] << std::endl; 
                }
            } else if (fileToExecute == "exit") {
                cout << "Bye!! Thanks for stopping by!" << endl;
                return 0;
            }

            vector<Redirection> redirections = commands[x].redirections;

            vector<const char*> totalArgPayload;

            totalArgPayload.push_back(fileToExecute.c_str());
            for (int x=0; x<args.size(); x++) {
                totalArgPayload.push_back(args[x].c_str());
            }
            totalArgPayload.push_back(NULL);

            int fd[2];
            pipe (fd);
            char *const * totalArgs = (char *const *) totalArgPayload.data();
            
            pid_t pid = fork();
            if (pid == 0) {
                // Handling file redirection; This has higher precedence than piping (I think this is the right approach?)
                for (int i=0; i<redirections.size(); i++) {
                    int fdRedir = open(redirections[i].fileName.c_str(), O_RDWR | O_CREAT, 0666);
                    if (redirections[i].type == READ_FROM) {
                        dup2(fdRedir, 0);
                    } else if (redirections[i].type == WRITE_TO) {
                        dup2(fdRedir, 1);
                    }
                }

                if (lastReadFd >=0) {
                    dup2(lastReadFd, 0); // take read end of pipe and put it into stdin (for next iteration)
                    // only do this in the child process as it destroys stdin
                }
                if (x < commands.size() - 1) {
                    dup2(fd[1], 1); // take the write end of pipe and put it into stdout
                }

                execvp(totalArgs[0], totalArgs);
                return 0;
            } else {
                if (x >= commands.size() - 1) {
                    if (isBackgroundLine) { // if this command chain has an "&" don't wait on it
                        backgroundProcesses.push_back(pid);
                    } else {
                        waitpid(pid, NULL, 0); // only wait on last pipe command
                    }
                }
                lastReadFd = fd[0];
                close(fd[1]);
            }
            // test one program at a time
        }
    }
}