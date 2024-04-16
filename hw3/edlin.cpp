/*
NAME:       EDLIN.CPP
AUTHOR:     SEBJIN KENNEDY
DATE:       APRIL 8, 2024
PURPOSE:    EDLIN LINE EDITOR IN C++

*/
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

void print(std::vector<std::string>);
void loadFile(std::vector<std::string>&, std::string);
void save(std::vector<std::string>, std::string);
void edit(std::vector<std::string>&, std::string);
void compile(std::string);
void run(std::string);

int main()
{
    std::vector<std::string> lines;      // vector holding the file itself
    std::string line;                    // string holding the file's lines 
    std::string fileName = "";           // setup fileName so it
    std::string input;                   // setup input used for all of program
    
    std::cout << "Welcome to Edlin\n"; // Send welcome Mmessage 
    do // Sset up inf while loop that runs edlin
    {
        std::cout << "edlin> "; // edlin output indicator
        std::getline(std::cin, input); // edlin input
        //****************PRINT**************//
        if (input.at(0) == 'p') // print the file from the vector holding the file's contents
            print(lines);
        //**************LOAD FILE***********//
        else if (input.at(0) == 'l') // load the file
        {
            lines.clear(); // clear the vector so there is no appending new data onto old
            fileName = input.substr(2, input.size()); // save the name of the file to fileName
            if (fileName.empty()) // check if the fileName is empty 
            {
                std::cout << "Please enter a file name\n";
                std::cerr << "No file name entered\n";
                continue; // do another check for input through the while loop
            } 
            loadFile(lines, fileName);
        }
        //************SAVE FILE*************//
        else if (input.at(0) == 's') // save to file
        {
            fileName = input.substr(2, fileName.size()); // save the name of the file
            if (fileName.empty()) // check if there is no inputted file
            {
                std::cout << "Please enter a file name\n"; 
                std::cerr << "No file name entered\n";
                continue; //jump back to start of the while loop to retry a filename
            }
            save(lines, fileName);
        }
        //************EDIT FILE************//
        else if (input.at(0) == 'e') // edit
        {
            if (input == "e" || input == "e ") // fail loop in case edit screwsu up
            {
                std::cout << "Please enter the line number you want to edit and what you want to change the line to in this format: \n";
                std::cout << "e <line number <what you're going to change the line to>\n";
                continue; 
            }
            edit (lines, input);
        }
        //************COMPILE**************//
        else if (input.at(0) == 'c') // compile
        {
            if (fileName.empty()) // check to see if the user has a fileName loaded into the program
            {
                std::cout << "\nPlease enter the file name of your source code: ";
                std::getline(std::cin, fileName); // cin will always take the first string inputted by a user,
                // fileName = fileName.substr(0, fileName.size()); 
            }
            compile(fileName);
        }
        //************RUN FILE**********//
        else if (input.at(0) == 'r') // run
        {
            if (fileName.empty())
            {
                std::cout << "Executable Name is empty.\n";
                std::cerr << "Executable File not Found.\n";
                std::cout << "Please load a file into memory first\n";
                continue; // Go back to start of loop to get a valid command
            }
            //fileName = input.substr(2, input.size()); // Assuming input format: r <executableFileName>

            run(fileName);
        }
        //*************QUIT**************//
        else if (input.at(0) == 'q')
            break;
        //**************DEFAULT*********//
        else // default if the user input is incorrect
        {
            std::cout << "Please enter a command from the following command list: \n";
            std::cout << "LOAD FILE:                     edlin> l <fileName>.cpp\n";
            std::cout << "PRINT:                         edlin> p\n";
            std::cout << "SAVE FILE TO FILE:             edlin> s <fileName>.cpp\n";
            std::cout << "EDIT FILE:                     edlin> e <lineNumber> <newLineYouWantToPutIn>\n";
            std::cout << "COMPILE FILE:                  edlin> c\n";
            std::cout << "RUN FILE:                      edlin> r <fileName>\n";

            std::cerr << "ERROR: Correct Command not chosen\n";
        }
    } while(true);
    return 0;
}

void print(std::vector<std::string> lines)
{
     for (int i = 0; i < lines.size(); i++)
        {
            std::cout << i + 1 << " " << lines[i] << std::endl;
        }
}
void loadFile(std::vector<std::string> &lines, std::string fname)
{
    std::ifstream inputFile;
    std::string line;
    inputFile.open(fname); // open the file within fileName
    while (!inputFile.eof() && std::getline(inputFile, line)) //get every line from the while loop
    {
        lines.push_back(line); // append line[i] into the lines vector
    }
    inputFile.close();
}
void save(std::vector<std::string> lines, std::string fname)
{
    std::ofstream outputFile;
    outputFile.open(fname, std::ofstream::trunc);//erase contents of the file using truncate
    for (auto line : lines) {
        outputFile << line << std::endl;
    }
    outputFile.close();
}
void edit(std::vector<std::string> &lines, std::string input)
{
    //int check = input.at(2) - '0'; // convert the integer at index 2 from integer to character 
    int check = stoi(input.substr(2, 2));
    if (check > lines.size()) //check the line number that you want to edit and see if it is bigger than the size 
    {
        lines.push_back(input.substr(4, input.size()));
    }
    else
        lines[check - 1] = (input.substr(4, input.size())); // if the line number is not bigger, do not append, but insert
}
void compile(std::string fileName)
{
    std::string exeConvert = ".exe";     // setup for compile
    int pid = fork();
    if (pid == 0) // Child process
    {
        // Prepare the command and arguments for compilation
        std::string command = "g++";
        //std::string outputFileName = fileName.substr(0, fileName.size()) + ".exe";
        std::string outputFileName = fileName.substr(0, fileName.size()) + ".exe"; // Removes extension if any and appends .exe if needed.
        char* argsvC [] =
        {
            (char *) "g++",// FIRST LOAD G++ INTO TERMINAL
            (char *) "-Wall",// THEN LOAD -WALL INTO TERMINAL
            (char *) fileName.c_str(),// THEN LOAD THE FILENAME (TRANSLATED INTO CSTRING) INTO TERMINAL
            (char *) "-o", // THEN LOAD THE RENAME FLAG INTO TERMINAL                    
            (char *) outputFileName.c_str(), // THEN LOAD THE EXECUTABLE NAME (TRANSLATED INTO CSTRING) INTO TERMINAL
            nullptr //LOAD NULLPTR AS LAST
        };
        char *const menvp[] = 
        {
            (char *) "PATH=/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin", 
            0 // SETUP MENVP
        };
        execvpe(command.c_str(), argsvC, menvp); // Use execvpe for file execution with PATH search

        // If execvp returns, it means it failed
        std::cerr << "ERROR: " << strerror(errno) << std::endl;
        exit(1); // Exit child process in case of failure to avoid continuing in the loop
    }
    else if (pid > 0) // Parent process
    {
        int status;
        wait(&status); // Wait for the child process to finish
    }
    else // fork failed
    {
        std::cerr << "ERROR: " << strerror(errno) << std::endl; //cerr if fork failed
    }
}
void run(std::string fileName)
{
    int pid = fork();
    if (pid == 0) // Child process
    {
        std::string use = fileName + ".exe"; // add .exe suffix
        // Construct the command for execution
        char *executePath = new char[use.size() + 3]; // Allocate space for "./" + use + null terminator
        strcpy(executePath, "./"); // Add the command that will run filename.cpp.exe--(this is just ./ which means run this file from the current directory)
        strcat(executePath, use.c_str()); // Append the file name

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
        std::cerr << "ERROR: " << strerror(errno) << std::endl;
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
        std::cerr << "ERROR: " << strerror(errno) << std::endl; // Fork failed
    }   
}
