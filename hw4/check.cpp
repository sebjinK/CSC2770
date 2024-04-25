#include <iostream>
#include <cstring>

int main()
{
    char buffer[1024] = "Hello.txt\n";

    std::cout << "Length: " << strlen(buffer) << std::endl;
    for(int i =0; i < strlen(buffer); i++)
    {
        std::cout << buffer[i] << " ";
    }

    std::cout << "\nSize: " << sizeof(buffer) << std::endl;



    return 0;
}