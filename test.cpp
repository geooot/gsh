#include "functions.h"
#include "assert.h"
#include <string>
#include <vector>
#include <iostream>

static const char* TOKEN_TYPE_MAP[] = {"EXECUTABLE", "ARGUMENT", "PIPE", "REDIRECT_OUT", "REDIRECT_IN", "REDIRECT_FILE"};

void printRedirectionList(const std::vector<Redirection>& list) {
    std::cout << "[";
    for (int x=0; x<list.size(); x++) {
        std::cout << "{type: " << list[x].type << ", fileName: \"" << list[x].fileName << "\"}, ";
    }
    std::cout << "]";
}

void printStringList(const std::vector<std::string>& list) {
    std::cout << "[";
    for (int x=0; x<list.size(); x++) {
        std::cout << "\"" << list[x] << "\", ";
    }
    std::cout << "]";
}

void printCommands(const std::vector<Command>& commands) {
    std::cout << "["; 
    for (int x=0; x<commands.size(); x++) {
        std::cout << "{fileToExecute: \"" << commands[x].fileToExecute << "\", ";
        std::cout << "args: ";
        printStringList(commands[x].args);
        std::cout << ", ";
        std::cout << "redirections: ";
        printRedirectionList(commands[x].redirections);
        std::cout << "}, ";
    }
    std::cout << "]" <<std::endl;
}

void printTokenList(const std::vector<Token>& list) {
    std::cout << "["; 
    for (int x=0; x<list.size(); x++) {
        std::cout << "{type: ";
        std::cout << TOKEN_TYPE_MAP[list[x].type];
        std::cout << ", data: \"";
        std::cout << list[x].data;
        std::cout << "\"},";
    }
    std::cout << "]" << std::endl;
}

void rtrim_test() {
    std::cout << "[rtrim] Starting Tests" << std::endl;
    std::string str = "hello world!\n  ";
    std::string expectedResult = "hello world!";

    std::string result = rtrim(str);

    std::cout << "[rtrim] Trial 1" << std::endl;
    assert(expectedResult == result);

    std::string str2 = "";
    std::string expectedResult2 = "";

    std::string result2 = rtrim(str2);

    std::cout << "[rtrim] Trial 2" << std::endl;
    assert(str2 == result2);

    std::string str3 = "hello";
    std::string expectedResult3 = "hello";

    std::string result3 = rtrim(str3);

    std::cout << "[rtrim] Trial 3" << std::endl;
    assert(str3 == result3);
    std::cout << "[rtrim] Finished All Tests" << std::endl;
}

void shouldLineBeDoneInBackground__test() {
    std::cout << "[shouldLineBeDoneInBackground] Starting Tests" << std::endl;
    std::cout << "[shouldLineBeDoneInBackground] Trial 1" << std::endl;
    assert(shouldLineBeDoneInBackground("dd if=/dev/zero of=/dev/null bs=1024 count=10485760 &"));
    std::cout << "[shouldLineBeDoneInBackground] Trial 2" << std::endl;
    assert(shouldLineBeDoneInBackground("sleep 10 &\n"));
    std::cout << "[shouldLineBeDoneInBackground] Trial 3" << std::endl;
    assert(!shouldLineBeDoneInBackground("ps −a | head −5\n"));
    std::cout << "[shouldLineBeDoneInBackground] Finished All Tests" << std::endl;
}

void splitIgnoreQuotes__test() {
    std::cout << "[splitIgnoreQuotes] Starting Tests" << std::endl;

    std::cout << "[splitIgnoreQuotes] Trial 1" << std::endl;
    std::vector<std::string> res1 = splitIgnoreQuotes("Hello World", ' ');
    assert(res1[0] == "Hello");
    assert(res1[1] == "World");

    std::cout << "[splitIgnoreQuotes] Trial 2" << std::endl;
    std::vector<std::string> res2 = splitIgnoreQuotes("echo \"Hello | World\" | ls -la", '|');
    assert(res2[0] == "echo \"Hello | World\" ");
    assert(res2[1] == " ls -la");


    std::cout << "[splitIgnoreQuotes] Trial 3" << std::endl;
    std::vector<std::string> res3 = splitIgnoreQuotes("echo \"Hello > 'World'\" > test.txt | ls -la", '>');
    assert(res3[0] == "echo \"Hello > 'World'\" ");
    assert(res3[1] == " test.txt | ls -la");

    std::cout << "[splitIgnoreQuotes] Trial 4" << std::endl;
    std::vector<char> list;
    list.push_back(' ');
    list.push_back('\t');
    std::vector<std::string> res4 = splitIgnoreQuotes("echo\t\"Hello > 'World'\" > test.txt | ls -la", list);
    assert(res4[0] == "echo");
    assert(res4[1] == "\"Hello > 'World'\" > test.txt | ls -la");

    std::cout << "[splitIgnoreQuotes] Trial 5" << std::endl;
    std::vector<std::string> res5 = splitIgnoreQuotes("Hello World", 'd');
    assert(res5[0] == "Hello Worl");
    assert(res5[1] == "");

    std::cout << "[splitIgnoreQuotes] Trial 6" << std::endl;
    std::vector<std::string> res6 = splitIgnoreQuotes("Hello World", 'i');
    assert(res6[0] == "Hello World");
    assert(res6[1] == "");

    std::cout << "[splitIgnoreQuotes] Trial 7" << std::endl;
    std::vector<char> splitList;
    list.push_back(' ');
    list.push_back('\t');
    list.push_back('\n');
    std::vector<std::string> res7 = splitIgnoreQuotes("\"<<<<< This message contains a line feed >>>>>\\n\"", splitList);
    assert(res7[0] == "\"<<<<< This message contains a line feed >>>>>\\n\"");
    assert(res7[1] == "");
    std::cout << "[splitIgnoreQuotes] Finished All Tests" << std::endl;
}

void parseLine__test(){
    std::vector<Token> tokens1 = parseLine("echo −e \"<<<<< This message contains a line feed >>>>>\\n\"");
    printTokenList(tokens1);
    std::cout << "[parseLine] Trial 1" << std::endl;
    assert(tokens1[0].type == EXECUTABLE);
    assert(tokens1[0].data == "echo");
    assert(tokens1[1].type == ARGUMENT);
    assert(tokens1[1].data == "−e");
    assert(tokens1[2].type == ARGUMENT);
    assert(tokens1[2].data == "<<<<< This message contains a line feed >>>>>\\n");

    std::vector<Token> tokens2 = parseLine("echo \"<<<<< Start to exercise pipe >>>>>\"");
    printTokenList(tokens2);
    std::cout << "[parseLine] Trial 2" << std::endl;
    assert(tokens2.size() == 2);

    std::vector<Token> tokens3 = parseLine("ps > test.txt");
    printTokenList(tokens3);
    std::cout << "[parseLine] Trial 3" << std::endl;
    assert(tokens3.size() == 3);

    std::vector<Token> tokens4 = parseLine("grep\tpts <\ntest.txt");
    printTokenList(tokens4);
    std::cout << "[parseLine] Trial 4" << std::endl;
    assert(tokens4.size() == 4);
    
    std::vector<Token> tokens5 = parseLine("pwd > pwd.txt");
    printTokenList(tokens5);
    std::cout << "[parseLine] Trial 5" << std::endl;
    assert(tokens5.size() == 3);

    std::vector<Token> tokens6 = parseLine("mv   pwd.txt  newpwd.txt");
    printTokenList(tokens6);
    std::cout << "[parseLine] Trial 6" << std::endl;
    assert(tokens6.size() == 3);

    std::vector<Token> tokens7 = parseLine("awk '{print $8$9}' < test.txt | head −10 | head −8 | head −7 | sort > output.txt");
    printTokenList(tokens7);
    std::cout << "[parseLine] Trial 7" << std::endl;
    assert(tokens7.size() == 17);
    std::cout << "[parseLine] Finished All Tests" << std::endl;
}

void tokensToCommands_test() {
    std::cout << "[tokensToCommands] Trial 1: \"" <<"echo −e \"<<<<< This message contains a line feed >>>>>\\n\""<<"\""<<  std::endl;
    std::vector<Token> tokens1 = parseLine("echo −e \"<<<<< This message contains a line feed >>>>>\\n\"");
    std::vector<Command> commands1 = tokensToCommands(tokens1);
    printCommands(commands1);

    std::cout << "[tokensToCommands] Trial 2: \"" <<"grep pts < test.txt"<<"\""<<  std::endl;
    std::vector<Token> tokens2 = parseLine("grep pts < test.txt");
    std::vector<Command> commands2 = tokensToCommands(tokens2);
    printCommands(commands2);

    std::cout << "[tokensToCommands] Trial 3: \"" <<"ls −l /proc/sys | awk \"{print $8}\" | head −3 | sort −r"<<"\""<<  std::endl;
    std::vector<Token> tokens3 = parseLine("ls −l /proc/sys | awk \"{print $8}\" | head −3 | sort −r");
    printTokenList(tokens3);
    std::vector<Command> commands3 = tokensToCommands(tokens3);
    printCommands(commands3);

    std::cout << "[tokensToCommands] Trial 4: \"" <<"cd ../"<<"\""<<  std::endl;
    std::vector<Token> tokens4 = parseLine("cd ../");
    printTokenList(tokens4);
    std::vector<Command> commands4 = tokensToCommands(tokens4);
    printCommands(commands4);

}


int main() {
    rtrim_test();
    std::cout << std::endl;
    shouldLineBeDoneInBackground__test();
    std::cout << std::endl;
    splitIgnoreQuotes__test();
    std::cout << std::endl;
    parseLine__test();
    std::cout << std::endl;
    tokensToCommands_test();
    return 0;
}