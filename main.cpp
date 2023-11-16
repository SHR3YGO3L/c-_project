#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>
#include <algorithm>

using namespace std;

namespace fs = filesystem;

// Structure to store tokenized words
struct Token {
    string word;
    Token* next;
};

// Structure to store file contents
struct FileContent {
    string fileName;
    Token* tokens;
    int occurrences;
};

// Function to tokenize a string into words
Token* tokenizeString(const string& content) {
    Token* tokens = nullptr;
    istringstream iss(content);
    string word;

    while (iss >> word) {
        Token* token = new Token{word, tokens};
        tokens = token;
    }

    return tokens;
}

int main() {
    vector<FileContent> fileContents;

    // Read text files and tokenize their contents
    //enter file path to data folder for D:\\project-cpp\\data
    string directoryPath = "D:\\project-cpp\\data";
    for (const auto& entry : fs::directory_iterator(directoryPath)) {
        if (entry.is_regular_file()) {
            string fileName = entry.path().filename().string();
            ifstream fileStream(entry.path());
            if (fileStream.is_open()) {
                string content((istreambuf_iterator<char>(fileStream)), istreambuf_iterator<char>());
                fileStream.close();
                Token* tokens = tokenizeString(content);
                fileContents.push_back({fileName, tokens, 0});
            }
        }
    }

    // Ask the user for a keyword to search
    string keyword;
    cout << "Enter a keyword to search: ";
    cin >> keyword;

    // Search for the keyword and count occurrences
    for (FileContent& fileContent : fileContents) {
        Token* currentToken = fileContent.tokens;
        while (currentToken) {
            if (currentToken->word == keyword) {
                fileContent.occurrences++;
            }
            currentToken = currentToken->next;
        }
    }

    // Filter files with at least one occurrence of the keyword
    vector<FileContent> filteredFiles;
    for (const FileContent& result : fileContents) {
        if (result.occurrences >= 1) {
            filteredFiles.push_back(result);
        }
    }

    // Rank files based on occurrences
    sort(
        filteredFiles.begin(),
        filteredFiles.end(),
        [](const FileContent& a, const FileContent& b)
        {
        return a.occurrences > b.occurrences;
        }
    );

    // Display search results
    if (filteredFiles.empty()) {
        cout << "No files with at least one occurrence of the keyword found." << endl;
    } else {
        int rank = 1;
        for (const FileContent& result : filteredFiles) {
            cout << "Rank " << rank << ": " << result.fileName << " (Occurrences: " << result.occurrences << " times)" << endl;
            rank++;
        }

        // Ask the user to select a file to open
        int choice;
        cout << "Enter the rank of the file you want to open: ";
        cin >> choice;
        if (choice >= 1 && choice <= filteredFiles.size()) {
            const FileContent& selectedFile = filteredFiles[choice - 1];
            cout << "Opening file: " << selectedFile.fileName << endl;

            // Open the selected file with the default program
            string openCommand = "start " + directoryPath + "\\" + selectedFile.fileName;
            system(openCommand.c_str());
        } else {
            cout << "Invalid choice. Please select a valid file rank." << endl;
        }
    }

    // Free the memory used by tokenized words
    for (FileContent& fileContent : fileContents) {
        Token* currentToken = fileContent.tokens;
        while (currentToken) {
            Token* nextToken = currentToken->next;
            delete currentToken;
            currentToken = nextToken;
        }
    }

    return 0;
}
