#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <string>

using namespace std;

// Node for the Huffman tree
class HuffmanNode
{
public:
    char data;
    int freq;
    HuffmanNode *left;
    HuffmanNode *right;

    HuffmanNode(char data, int freq)
    {
        this->data = data;
        this->freq = freq;
        left = right = nullptr;
    }
};

// Comparator for the priority queue
class Compare
{
public:
    bool operator()(HuffmanNode *a, HuffmanNode *b)
    {
        return a->freq > b->freq;
    }
};

// Build the frequency table
unordered_map<char, int> buildFrequencyTable(const string &data)
{
    unordered_map<char, int> freqMap;
    for (char ch : data)
    {
        freqMap[ch]++;
    }
    return freqMap;
}

// Build the Huffman tree
HuffmanNode *buildHuffmanTree(unordered_map<char, int> &freqMap)
{
    priority_queue<HuffmanNode *, vector<HuffmanNode *>, Compare> minHeap;

    for (auto pair : freqMap)
    {
        minHeap.push(new HuffmanNode(pair.first, pair.second));
    }

    while (minHeap.size() != 1)
    {
        HuffmanNode *left = minHeap.top();
        minHeap.pop();
        HuffmanNode *right = minHeap.top();
        minHeap.pop();

        HuffmanNode *newNode = new HuffmanNode('\0', left->freq + right->freq);
        newNode->left = left;
        newNode->right = right;
        minHeap.push(newNode);
    }

    return minHeap.top();
}

// Generate Huffman codes from the Huffman tree
void generateHuffmanCodes(HuffmanNode *root, string code, unordered_map<char, string> &huffmanCodes)
{
    if (!root)
        return;

    if (root->data != '\0')
    {
        huffmanCodes[root->data] = code;
    }

    generateHuffmanCodes(root->left, code + "0", huffmanCodes);
    generateHuffmanCodes(root->right, code + "1", huffmanCodes);
}

// Compress the input data using Huffman codes
string compressData(const string &data, unordered_map<char, string> &huffmanCodes)
{
    string compressedData = "";
    for (char ch : data)
    {
        compressedData += huffmanCodes[ch];
    }
    return compressedData;
}

// Decompress the encoded data using the Huffman tree
string decompressData(const string &compressedData, HuffmanNode *root)
{
    string decompressedData = "";
    HuffmanNode *currentNode = root;
    for (char bit : compressedData)
    {
        if (bit == '0')
        {
            currentNode = currentNode->left;
        }
        else
        {
            currentNode = currentNode->right;
        }

        if (!currentNode)
        { // Check for nullptr
            cerr << "Error: Reached a nullptr in the Huffman tree during decompression." << endl;
            break; // Prevent dereferencing nullptr
        }

        if (!currentNode->left && !currentNode->right)
        {
            decompressedData += currentNode->data;
            currentNode = root;
        }
    }
    return decompressedData;
}

// Save compressed data and Huffman codes to file
void saveCompressedFile(const string &compressedData, unordered_map<char, string> &huffmanCodes, const string &filename)
{
    ofstream outFile(filename, ios::binary);
    for (auto pair : huffmanCodes)
    {
        outFile << pair.first << ":" << pair.second << "\n";
    }
    outFile << "===" << endl;
    outFile << compressedData;
    outFile.close();
}

// Load compressed data and Huffman codes from file
void loadCompressedFile(string &compressedData, unordered_map<char, string> &huffmanCodes, const string &filename)
{
    ifstream inFile(filename, ios::binary);
    string line;
    while (getline(inFile, line))
    {
        if (line == "===")
            break;
        if (line.empty())
            continue; // Avoid processing empty lines
        char ch = line[0];
        string code = line.substr(2);
        huffmanCodes[ch] = code;
    }
    getline(inFile, compressedData);
    inFile.close();
}

int main()
{
    // Open file and read input
    ifstream inputFile("input.txt");
    if (!inputFile)
    { // Check if file opened successfully
        cerr << "Error opening input file!" << endl;
        return 1;
    }

    string inputText((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
    inputFile.close();

    if (inputText.empty())
    { // Check if the input text is empty
        cerr << "Error: Input file is empty!" << endl;
        return 1;
    }

    // Build the frequency table
    unordered_map<char, int> freqMap = buildFrequencyTable(inputText);

    // Build the Huffman tree
    HuffmanNode *root = buildHuffmanTree(freqMap);

    // Generate Huffman codes
    unordered_map<char, string> huffmanCodes;
    generateHuffmanCodes(root, "", huffmanCodes);

    // Compress the data
    string compressedData = compressData(inputText, huffmanCodes);
    saveCompressedFile(compressedData, huffmanCodes, "compressed.huf");

    cout << "Data compressed and saved to 'compressed.huf'." << endl;

    // Decompress the data
    string loadedCompressedData;
    unordered_map<char, string> loadedHuffmanCodes;
    loadCompressedFile(loadedCompressedData, loadedHuffmanCodes, "compressed.huf");

    // Ensure the tree is still valid
    if (root == nullptr)
    {
        cerr << "Error: Huffman tree is null!" << endl;
        return 1;
    }

    string decompressedData = decompressData(loadedCompressedData, root);

    cout << "Decompressed Data: " << decompressedData << endl;

    return 0;
}
