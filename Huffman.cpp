#include <bits/stdc++.h>

using namespace std;

struct Node {
    char ch;
    int freq;
    Node *left, *right;

    Node(char ch, int freq, Node* left = nullptr, Node* right = nullptr) 
        : ch(ch), freq(freq), left(left), right(right) {}
};

struct Compare {
    bool operator()(Node* left, Node* right) {
        return left->freq > right->freq;
    }
};

class HuffmanCoding {
public:
    void compress(const string& inputFile, const string& outputFile);
    void decompress(const string& inputFile, const string& outputFile);

private:
    void buildHuffmanTree(const string& text);
    void encode(Node* root, const string& str, unordered_map<char, string>& huffmanCode);
    void decode(Node* root, int& index, const string& str, string& output);
    void saveTree(Node* root, ostream& out);
    Node* loadTree(istream& in);

    unordered_map<char, string> huffmanCode;
    Node* root = nullptr;
};

void HuffmanCoding::buildHuffmanTree(const string& text) {
    unordered_map<char, int> freq;
    for (char ch : text) {
        freq[ch]++;
    }

    priority_queue<Node*, vector<Node*>, Compare> pq;
    for (auto pair : freq) {
        pq.push(new Node(pair.first, pair.second));
    }

    while (pq.size() != 1) {
        Node *left = pq.top(); pq.pop();
        Node *right = pq.top(); pq.pop();
        int sum = left->freq + right->freq;
        pq.push(new Node('\0', sum, left, right));
    }

    root = pq.top();
    encode(root, "", huffmanCode);
}

void HuffmanCoding::encode(Node* root, const string& str, unordered_map<char, string>& huffmanCode) {
    if (root == nullptr) return;

    if (!root->left && !root->right) {
        huffmanCode[root->ch] = str;
    }

    encode(root->left, str + "0", huffmanCode);
    encode(root->right, str + "1", huffmanCode);
}

void HuffmanCoding::saveTree(Node* root, ostream& out) {
    if (root == nullptr) {
        out.put('#'); // Marker for null
        return;
    }
    out.put(root->ch);
    saveTree(root->left, out);
    saveTree(root->right, out);
}

Node* HuffmanCoding::loadTree(istream& in) {
    char ch = in.get();
    if (ch == '#') {
        return nullptr;
    }
    Node* node = new Node(ch, 0);
    node->left = loadTree(in);
    node->right = loadTree(in);
    return node;
}

void HuffmanCoding::compress(const string& inputFile, const string& outputFile) {
    ifstream in(inputFile, ios::binary);
    string text((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
    in.close();

    buildHuffmanTree(text);

    string str = "";
    for (char ch : text) {
        str += huffmanCode[ch];
    }

    // Huffman tree and encoded string as bytes
    ofstream out(outputFile, ios::binary);
    saveTree(root, out);
    out.put('|');        // Separator between tree and data

    //padding information
    int padding = 8 - (str.size() % 8);
    if (padding == 8) padding = 0;
    out.put(static_cast<char>(padding + '0'));

    for (size_t i = 0; i < str.size(); i += 8) {
        bitset<8> bits(str.substr(i, 8));
        out.put(static_cast<unsigned char>(bits.to_ulong()));
    }

    out.close();
}

void HuffmanCoding::decode(Node* root, int& index, const string& str, string& output) {
    if (root == nullptr) return;

    if (!root->left && !root->right) {
        if(root->ch != '\n') output += root->ch;
        return;
    }

    index++;
    if (index >= str.size()) return;

    if (str[index] == '0') {
        decode(root->left, index, str, output);
    } else {
        decode(root->right, index, str, output);
    }
}

void HuffmanCoding::decompress(const string& inputFile, const string& outputFile) {
    ifstream in(inputFile, ios::binary);
    root = loadTree(in);
    char separator = '|';
    in.get(separator); // Read the separator '|'

    // Read padding information
    int padding = in.get() - '0';

    string encodedText;
    unsigned char byte;
    while (in.read(reinterpret_cast<char*>(&byte), sizeof(byte))) {
        bitset<8> bits(byte);
        encodedText += bits.to_string();
    }
    in.close();

    // Remove the padding bits
    if (padding > 0) {
        encodedText.erase(encodedText.end() - padding, encodedText.end());
    }

    string output = "";
    int index = -1;
    while (index < (int)encodedText.size() - 1) {
        decode(root, index, encodedText, output);
    }

    ofstream out(outputFile);
    out << output;
    out.close();
}

// Public function to free the Huffman Tree

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " <compress|decompress> <input file> <output file>" << endl;
        return 1;
    }

    string mode = argv[1];
    string inputFile = argv[2];
    string outputFile = argv[3];

    HuffmanCoding hc;

    if (mode == "compress") {
        hc.compress(inputFile, outputFile);
    } else if (mode == "decompress") {
        hc.decompress(inputFile, outputFile);
    } else {
        cerr << "Invalid mode: " << mode << endl;
        return 1;
    }
    return 0;
}
