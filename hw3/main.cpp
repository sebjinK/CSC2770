#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::string;
using std::getline;

//const int SIZE = 1024;

int main()
{
    std::vector<string> lines;      // vector holding the file itself
    string line;                    // string holding the file's lines 
    std::ifstream inputFile;        // setup input file for load
    std::ofstream outputFile;       // setup output file for save
    string fileName = "";           // setup fileName so it
    char * fileNameArr;
    char * executableFileNameArr;
    string input;
    string exeConvert = ".exe";
    
    //char* inputArray = new char[SIZE];
    
    cout << "Welcome to Edlin\n"; // Send welcome Mmessage 
    cout << "edlin> "; // initial output to deal with input being initialize before while loop starts
    getline(cin, input); // initial input to deal with input being initialized before while loop starts
    while (true) // Sset up inf while loop that runs edlin
    {
        if (input.at(0) == 'p') // print the file from the vector that saved the file
        {
            for (int i = 0; i < lines.size(); i++)
            {
                cout << i + 1 << " " << lines[i] << endl;
            }
            //loop over given lines and print to the screen
        }
        else if (input.at(0) == 'l') // load the file
        {
            lines.clear(); // clear the vector so there is no appending new data onto old
            fileName = input.substr(2, input.size()); // save the name of the file to fileName
            if (fileName.empty()) // check if the fileName is empty 
            {
                cout << "Please enter a file name\n";
                cerr << "No file name entered\n";
                continue; // do another check for input through the while loop
            } 
            inputFile.open(fileName); // open the file within fileName
            while (!inputFile.eof() && getline(inputFile, line)) //get every line from the while loop
            {
                lines.push_back(line); // append line[i] into the lines vector
            }
        }
        else if (input.at(0) == 's') // save to file
        {
            fileName = input.substr(2, fileName.size()); // save the name of the file
            if (fileName.empty()) // check if there is no inputted file
            {
                cout << "Please enter a file name\n"; 
                cerr << "No file name entered\n";
                continue; //jump back to start of the while loop to retry a filename
            }
            outputFile.open(fileName, std::ofstream::trunc);//erase contents of the file using truncate
            for (auto line : lines) {
                outputFile << line << endl;
            }
        }
        else if (input.at(0) == 'e') // edit
        {
            int check = input.at(2) - '0'; // convert the integer at index 2 from integer to character 
            if (check > lines.size()) //check the line number that you want to edit and see if it is bigger than the size 
                lines.push_back(input.substr(4, input.size())); // if the line number is bigger,  append the line to the data structure
            else
                lines[check - 1] = (input.substr(4, input.size())); // if the line number is not bigger, do not append, but insert
        }

        else if (input.at(0) == 'c') // compile
        {
            
            if (fileName.empty()) // check to see if the user has a fileName loaded into the program
            {
                cout << "\nPlease enter the file name of your source code: ";
                cin >> fileName; // cin will always take the first string inputted by a user,
                // fileName = fileName.substr(0, fileName.size()); 
            }
            int pid = fork();
            if (pid == 0) // Child process
            {
                // Prepare the command and arguments for compilation
                string command = "g++";
                //string outputFileName = input.substr(2, input.size()) + ".exe";
                string outputFileName = fileName.substr(0, fileName.find_last_of(".")) + ".exe"; // Removes extension if any and appends .exe if needed
                std::vector<string> args = {command, "-o", outputFileName.c_str(), fileName.c_str()};

                // Convert vector<string> to char* array required for execvp
                std::vector<char*> argsvC;
                for (auto& arg : args) 
                {
                    argsvC.push_back(&arg.front());
                }
                argsvC.push_back(nullptr); // execvp expects a NULL terminated array
                char *const menvp[] = 
                {
	                (char *) "PATH=/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin", 
                    0
                };
                execvpe(command.c_str(), argsvC.data(), menvp);
                // Only if execvp fails
                cerr << "Compilation failed: " << strerror(errno) << endl;
                exit(1); // Exit child process in case of failure to avoid continuing in the loop
            }
            else if (pid > 0) // Parent process
            {
                int status;
                wait(&status); // Wait for the child process to finish
            }
            else // fork failed
            {
                cerr << "ERROR: " << strerror(errno) << endl;
            }
        }

        else if (input.at(0) == 'r') // run
        {
            fileName = input.substr(2, input.size()); // Assuming input format: r <executableFileName>
            if (fileName.empty())
            {
                cout << "Executable Name is empty.\n";
                cerr << "Executable File not Found.\n";
                continue; // Go back to start of loop to get a valid command
            }

            int pid = fork();
            if (pid == 0) // Child process
            {
                // Construct the command for execution
                char *executePath = new char[fileName.size() + 3]; // Allocate space for "./" + fileName + null terminator
                strcpy(executePath, "./"); // Add the command that will run filename.cpp.exe--(this is just ./ which means run this file from the current directory)
                strcat(executePath, fileName.c_str()); // Append the file name

                char *args[] = 
                {
                    executePath, 
                    nullptr // Argument list must end with NULL
                }; 
                char *const menvp[] = {
	                (char *) "PATH=/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin", 0
                };
                // Execute the program
                execvpe(executePath, args, menvp); // Use execvpe for file execution with PATH search

                // If execvp returns, it means it failed
                cerr << "ERROR: " << strerror(errno) << endl;
                delete[] executePath; // Clean up dynamically allocated memory
                exit(1); // Exit the child process with an error code
            }
            else if (pid > 0) // Parent process
            {
                int status;
                wait(&status); // Wait for the child process to finish
            }
            else
            {
                cerr << "ERROR: " << strerror(errno) << endl; // Fork failed
            }
        }

        else if (input.at(0) == 'q')
            break;
        
        else // default if the user input is incorrect
        {
            cout << "Please enter a command from the following command list: \n";
            cout << "LOAD FILE:                     edlin> l <fileName>.cpp\n";
            cout << "PRINT:                         edlin> p\n";
            cout << "SAVE FILE TO FILE:             edlin> s <fileName>.cpp\n";
            cout << "EDIT FILE:                     edlin> e <lineNumber> <newLineYouWantToPutIn>\n";
            cout << "COMPILE FILE:                  edlin> c\n";
            cout << "RUN FILE:                      edlin> r <fileName>\n";
            cout << "QUIT                           edlin> q\n";
            cout << "\n";
            cerr << "ERROR: Correct Command not chosen\n";
        }
        outputFile.close();
        inputFile.close();
        cout << "edlin> "; // edlin output indicator
        getline(cin, input); // edlin input
    }
    return 0;
}