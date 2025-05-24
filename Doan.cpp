#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <queue>
#include <iomanip>
#include <algorithm>
#include <fstream>

using namespace std;

const int BEAM_WIDTH = 5;

// Cau truc du lieu mot gia thuyet dich
struct Hypothesis {
    vector<string> translation;  // Cau da dich den hien tai
    double probability;          // Xac suat tich luy

    bool operator<(const Hypothesis& other) const {
        // Sap xep theo xac suat giam dan
        return probability < other.probability;
    }

    void print() const {
        for (const string& word : translation) {
            cout << word << " ";
        }
        cout << "(Score: " << fixed << setprecision(4) << probability << ")";
    }
};

// Tu dien anh xa tu tieng Anh sang tieng Viet kem xac suat
map<string, vector<pair<string, double>>> translation_dictionary = {
    {"hello",   {{"xin chao", 0.6}}}, 
    {"world",   {{"the gioi", 0.3}}}, 
    {"good",    {{"tot", 0.7}, {"ngon", 0.3}}},
    {"morning", {{"buoi sang", 0.5}}},
    {"my",      {{"cua minh", 0.6}}},
    {"friend",  {{"ban", 0.8}, {"nguoi ban", 0.2}}},
    {"how",     {{"the nao", 1.0}}},
    {"are",     {{"dang", 1.0}}},
    {"you",     {{"ban", 1.0}}},
    {"i",       {{"toi", 1.0}}},
    {"am",      {{"dang", 1.0}}},
    {"fine",    {{"on", 1.0},{"khoe",0.3}}},
};

// Ham tach chuoi thanh cac t
vector<string> tokenize(const string& sentence) {
    vector<string> tokens;
    stringstream ss(sentence);
    string word;
    while (ss >> word) {
        tokens.push_back(word);
    }
    return tokens;
}

// Ham Beam Search: thuc hien dich cau
vector<Hypothesis> beam_search_translate(const vector<string>& source) {
    priority_queue<Hypothesis> beam;
    beam.push({{}, 1.0});  // Gia thuyet ban dau la cau rieng

    for (const string& word : source) {
        priority_queue<Hypothesis> next_beam;

        auto translations = translation_dictionary.find(word);
        if (translations == translation_dictionary.end()) {
            // Neu ta khong co trong tu dien, giu nguyen
            translations = translation_dictionary.insert({word, {{word, 1.0}}}).first;
        }

        while (!beam.empty()) {
            Hypothesis hyp = beam.top();
            beam.pop();

            for (const auto& candidate : translations->second) {
                Hypothesis new_hyp = hyp;
                new_hyp.translation.push_back(candidate.first);
                new_hyp.probability *= candidate.second;

                next_beam.push(new_hyp);

                // Giu kích thuoc Beam giai hon
                if ((int)next_beam.size() > BEAM_WIDTH) {
                    next_beam.pop();
                }
            }
        }

        beam = next_beam;
    }

    // Thu thap tat ca beam còn lai
    vector<Hypothesis> results;
    while (!beam.empty()) {
        results.push_back(beam.top());
        beam.pop();
    }

    // Sap xep lai theo xác suat giam dan
    sort(results.rbegin(), results.rend());

    return results;
}

// Hàm in banner gioi thieu
void print_banner() {
    cout << "==============================\n";
    cout << " DICH MAY BEAM SEARCH \n";
    cout << "==============================\n";
}

// Hàm doc câu thanh file
vector<string> read_sentences_from_file(const string& filename) {
    vector<string> sentences;
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        if (!line.empty()) {
            sentences.push_back(line);
        }
    }
    return sentences;
}

// Hàm chính
int main() {
    print_banner();

    cout << "Chon che do:\n";
    cout << "1. Dich tu ban phim\n";
    cout << "2. Dich tu file (input.txt)\n";
    int mode;
    cout << "Nhap lua chon (1/2): ";
    cin >> mode;
    cin.ignore();

    vector<string> sentences;

    if (mode == 1) {
        string input;
        cout << "Nhap cau tieng Anh: ";
        getline(cin, input);
        sentences.push_back(input);
    } else if (mode == 2) {
        sentences = read_sentences_from_file("input.txt");
        if (sentences.empty()) {
            cerr << "File rong hoac khong ton tai.\n";
            return 1;
        }
    } else {
        cerr << "Lua chon khong hop le.\n";
        return 1;
    }

    // Dich tung cau
    for (const string& sentence : sentences) {
        cout << "\nCau goc: " << sentence << endl;
        vector<string> tokens = tokenize(sentence);
        vector<Hypothesis> results = beam_search_translate(tokens);

        cout << "==> Cac ket qua Beam Search:\n";
        for (size_t i = 0; i < results.size(); ++i) {
            cout << " [" << i+1 << "] ";
            results[i].print();
            cout << endl;
        }

        cout << "==> Cau dich chon (tot nhat): ";
        for (const string& word : results[0].translation) {
            cout << word << " ";
        }
        cout << endl;
    }

    return 0;
}

