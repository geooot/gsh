#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <string>
#include <iostream>
#include <vector>

enum RedirectionType {
    NONE,
    READ_FROM,
    WRITE_TO
};

enum TokenType {
    EXECUTABLE,
    ARGUMENT,
    PIPE,
    REDIRECT_OUT,
    REDIRECT_IN,
    REDIRECT_FILE
};

struct Redirection {
    RedirectionType type;
    std::string fileName;
};

struct Command {
    std::string fileToExecute;
    std::vector<std::string> args;
    std::vector<Redirection> redirections;
};

struct Token {
    TokenType type;
    std::string data;
};

/**
 * @breif	Takes a lines string and returns a list of tokens parsed from string
 * 
 * @param	line	
 * @return	std::vector<Token>	
 */
std::vector<Token> parseLine(const std::string& line);

/**
 * @breif	Given a line the shell needs to execute,
 *          this function determines if it should be done in the background or not
 * 
 * @param	line	line given to the shell
 * @return	true	this line should be handled in the background
 * @return	false	this line should be handled in the forground
 */
bool shouldLineBeDoneInBackground(const std::string& line);

/**
 * @breif	Given a list of tokens this function returns a list of commands (that need to be exec'ed and piped)
 * 
 * @param	line	line givin to the shell
 * @return	std::vector<Command>	list of commands that need to be further parsed
 */
std::vector<Command> tokensToCommands(const std::vector<Token>& tokens); 

///// GENERAL HELPERS

/**
 * @breif	Poor mans right trim.
 *          Takes a string and returns the string with any trailing spaces or newlines removed
 * 
 * @param	line	string to rtrim
 * @return	std::string trimmed string
 */
std::string rtrim(const std::string& line);

/**
 * @breif	Poor mans left trim.
 *          Takes a string and returns the string with any leading spaces or newlines removed
 * 
 * @param	line	string to ltrim
 * @return	std::string trimmed string
 */
std::string ltrim(const std::string& line);

/**
 * @breif	Takes a string and splits the string into two at the first sight of splitAt (unless its in quotes).
 * 
 * @param	line	
 * @param	splitAt	
 * @return	std::vector<std::string>	
 */
std::vector<std::string> splitIgnoreQuotes(const std::string& line, const char& splitAt); 

/**
 * @breif	Takes a string and splits the string into two at the first sight of splitAt (unless its in quotes).
 * 
 * @param	line	
 * @param	splitAt	
 * @return	std::vector<std::string>	
 */
std::vector<std::string> splitIgnoreQuotes(const std::string& line, const std::vector<char>& splitAt); 

/**
 * @breif	Remove Quotes from string if there are quotes
 * 
 */
std::string removeQuotes(const std::string& str);

#endif