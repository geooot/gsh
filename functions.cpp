#include "functions.h"


bool shouldLineBeDoneInBackground(const std::string& line) {
    return rtrim(line).back() == '&';
}

std::vector<Command> tokensToCommands(const std::vector<Token>& tokens) {
    std::vector<Command> commands;
    Command currentCmd;
    for (int x=0; x<tokens.size(); x++) {
        if (tokens[x].type == EXECUTABLE) {
            currentCmd.fileToExecute = tokens[x].data;
        } else if (tokens[x].type == ARGUMENT) {
            currentCmd.args.push_back(tokens[x].data);
        } else if (tokens[x].type == REDIRECT_FILE && x > 0 && tokens[x - 1].type == REDIRECT_IN) {
            Redirection r;
            r.fileName = tokens[x].data;
            r.type = READ_FROM;
            currentCmd.redirections.push_back(r);
        } else if (tokens[x].type == REDIRECT_FILE && x > 0 && tokens[x - 1].type == REDIRECT_OUT) {
            Redirection r;
            r.fileName = tokens[x].data;
            r.type = WRITE_TO;
            currentCmd.redirections.push_back(r);
        }
        if (tokens[x].type == PIPE || x >= tokens.size() - 1) {
            commands.push_back(currentCmd);
            currentCmd.args.clear();
            currentCmd.redirections.clear();
            currentCmd.fileToExecute = "";
        }
    }
    return commands;
}

std::string rtrim(const std::string& line) {
    size_t size = line.size();
    int endindex = 0;
    for (size_t x = size - 1; x >= 0; x--) {
        if (line[x] != '\n' && line[x] != ' ') {
            endindex = x;
            break;
        }
    }
    return line.substr(0, endindex + 1);
}

std::string ltrim(const std::string& line) {
    size_t size = line.size();
    int startindex = 0;
    for (size_t x = 0; x < size; x++) {
        if (line[x] != '\n' && line[x] != ' ') {
            startindex = x;
            break;
        }
    }
    return line.substr(startindex);
}

std::vector<std::string> splitIgnoreQuotes(const std::string& line, const char& splitAt) {
    std::vector<char> list;
    list.push_back(splitAt);
    return splitIgnoreQuotes(line, list);
}


std::vector<std::string> splitIgnoreQuotes(const std::string& line, const std::vector<char>& splitAt) {
    std::string before = line;
    std::string after = "";

    bool inQuotes = false;
    char quote = '\0';

    for (int x=0; x<line.size(); x++) {
        // check quote situation
        if (line[x] == '\"') {
            if (inQuotes && line[x] == quote) {
                inQuotes = false;
                quote = '\0';
            } else if(!inQuotes) {
                inQuotes = true;
                quote = '\"';
            }
        }
        if (line[x] == '\'') {
            if (inQuotes && line[x] == quote) {
                inQuotes = false;
                quote = '\0';
            } else if(!inQuotes) {
                inQuotes = true;
                quote = '\'';
            }
        }

        bool atSplit = false;
        for (int i=0; i<splitAt.size(); i++) {
            if (line[x] == splitAt[i]) {
                atSplit = true;
                break;
            }
        }

        // actually check split
        if (atSplit && !inQuotes) {
            before = line.substr(0, x);
            after = line.substr(x+1);
            break;
        }
    }

    std::vector<std::string> result;
    result.push_back(before);
    result.push_back(after);
    return result;
}

std::string removeQuotes(const std::string& str) {
    if (str == "\"\"" || str == "\'\'") {
        return "";
    }

    if (
        (str[0] == '\"' && str[str.size() - 1] == '\"') || 
        (str[0] == '\'' && str[str.size() - 1] == '\'')
    ) {
        return str.substr(1,str.size()-2);
    } else {
        return str;
    }
}

std::vector<Token> parseLine(const std::string& ln) {
    std::string line = rtrim(ltrim(ln)); 

    std::vector<Token> tokens;

    std::vector<char> splitPoints;
    splitPoints.push_back(' ');
    splitPoints.push_back('\t');
    splitPoints.push_back('\n');

    std::vector<std::string> splitted = splitIgnoreQuotes(line, splitPoints);

    while(splitted[0] != ""){
        std::string tokenString = rtrim(ltrim(splitted[0]));
        Token tk;
        if (tokens.size() <= 0 || tokens[tokens.size() - 1].type == PIPE) {
            // this token is a executable
            tk.data = tokenString;
            tk.type = EXECUTABLE;
        } else if (tokenString == "|") {
            // this token is a pipe
            tk.data = tokenString;
            tk.type = PIPE;
        } else if (tokenString == ">") {
            // this token is a redirect out
            tk.data = tokenString;
            tk.type = REDIRECT_OUT;
        } else if (tokenString == "<") {
            // this token is a redirect in
            tk.data = tokenString;
            tk.type = REDIRECT_IN;
        } else if (tokens.size() > 0 && (tokens[tokens.size() - 1].type == REDIRECT_IN || tokens[tokens.size() - 1].type == REDIRECT_OUT)) {
            // this token is a redirect file
            tk.data = removeQuotes(tokenString);
            tk.type = REDIRECT_FILE;
        } else {
            // this token is an argument
            tk.data = removeQuotes(tokenString);
            tk.type = ARGUMENT;
        }
        tokens.push_back(tk);

        splitted = splitIgnoreQuotes(ltrim(splitted[1]), splitPoints);
    }

    return tokens;
}