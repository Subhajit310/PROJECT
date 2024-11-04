#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include <cctype>
#include <cmath>

using namespace std;

// Helper function to clean and normalize a word
string normalizeWord(string word) {
    string result;
    for (char c : word) {
        if (isalnum(c)) {
            result += toupper(c); // Convert to uppercase
        }
    }
    return result;
}

// Function to check if the word is a common word to exclude
bool isCommonWord(const string& word) {
    static set<string> commonWords = {"A", "AND", "AN", "OF", "IN", "THE"};
    return commonWords.count(word) > 0;
}

// Function to extract word frequencies from a file
map<string, int> extractWordFrequencies(const string& filename) {
    ifstream file(filename);
    string word;
    map<string, int> wordCount;

    while (file >> word) {
        string normalizedWord = normalizeWord(word);
        if (!normalizedWord.empty() && !isCommonWord(normalizedWord)) {
            wordCount[normalizedWord]++;
        }
    }

    return wordCount;
}

// Function to normalize word frequencies by total number of words
map<string, double> normalizeFrequencies(const map<string, int>& wordCount) {
    map<string, double> normalizedFreq;
    int totalWords = 0;
    for (const auto& pair : wordCount) {
        totalWords += pair.second;
    }
    for (const auto& pair : wordCount) {
        normalizedFreq[pair.first] = (double)pair.second / totalWords;
    }
    return normalizedFreq;
}

// Function to compute similarity index between two books
double computeSimilarity(const map<string, double>& freq1, const map<string, double>& freq2) {
    double similarity = 0.0;
    for (const auto& pair : freq1) {
        const string& word = pair.first;
        if (freq2.count(word)) {
            similarity += min(freq1.at(word), freq2.at(word));
        }
    }
    return similarity;
}

// Function to find top N frequent words
vector<pair<string, double>> findTopNWords(const map<string, double>& freq, int N) {
    vector<pair<string, double>> words(freq.begin(), freq.end());
    sort(words.begin(), words.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });
    if (words.size() > N) {
        words.resize(N);
    }
    return words;
}

int main() {
    vector<string> filenames = {
        // Add the list of file names here (e.g., "Book1.txt", "Book2.txt", ...)
    };

    int numBooks = filenames.size();
    vector<map<string, double>> bookFrequencies(numBooks);

    // Step 1: Extract word frequencies and normalize them for each book
    for (int i = 0; i < numBooks; i++) {
        auto wordCount = extractWordFrequencies(filenames[i]);
        auto normalizedFreq = normalizeFrequencies(wordCount);

        // Get top 100 frequent words
        bookFrequencies[i] = map<string, double>();
        auto topWords = findTopNWords(normalizedFreq, 100);
        for (const auto& word : topWords) {
            bookFrequencies[i][word.first] = word.second;
        }
    }

    // Step 2: Create a 64x64 similarity matrix
    vector<vector<double>> similarityMatrix(numBooks, vector<double>(numBooks, 0));

    for (int i = 0; i < numBooks; i++) {
        for (int j = i + 1; j < numBooks; j++) {
            similarityMatrix[i][j] = computeSimilarity(bookFrequencies[i], bookFrequencies[j]);
            similarityMatrix[j][i] = similarityMatrix[i][j]; // Symmetric matrix
        }
    }

    // Step 3: Find top 10 most similar pairs
    vector<tuple<int, int, double>> similarPairs;
    for (int i = 0; i < numBooks; i++) {
        for (int j = i + 1; j < numBooks; j++) {
            similarPairs.push_back({i, j, similarityMatrix[i][j]});
        }
    }

    sort(similarPairs.begin(), similarPairs.end(), [](const auto& a, const auto& b) {
        return get<2>(a) > get<2>(b);
    });

    // Step 4: Report top 10 similar pairs
    cout << "Top 10 most similar book pairs:\n";
    for (int i = 0; i < 10 && i < similarPairs.size(); i++) {
        int book1 = get<0>(similarPairs[i]);
        int book2 = get<1>(similarPairs[i]);
        double similarity = get<2>(similarPairs[i]);
        cout << "Book " << book1 << " and Book " << book2 << " - Similarity: " << similarity << endl;
    }

    return 0;
}
