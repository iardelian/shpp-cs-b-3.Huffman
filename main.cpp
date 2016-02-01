#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <fstream>
#include <tuple>
#include <string>

using namespace std;

/*
* Class of Huffman node tree
*/
class Node{
public:
    int number; //frequency of occurrence of symbol in the file
    char symbol; //value (only for leaves)
    Node *left, *right; //pointers to the left and right sons
    
    Node(){ // constructor overload
        left = right = NULL;
    }
    
    Node (Node *L, Node *R){ //constructor takes a reference to the left and right son
        left = L;
        right = R;
        number = L->number + R->number; //the sum of two nodes values
    }
    
    ~Node(){
        delete left;
        delete right;
    }
};

/*
* Struct for sorting elements in the list
*/
struct MyCompare{
    bool operator()(Node* left, Node* right){ //parameters overload
        return left->number < right->number;
    }
};

class Helper{
public:
    static map <char,int> getFrequency(vector<char> list)
    {
        map <char,int> symbolFrequency;
        for (const auto &x: list)
            symbolFrequency[x]++;
        
        return symbolFrequency;
    }
};

class Huffman{
private:
    static const int countByte = 8;
    
    map <char, vector <bool>> symbolCodeTable; //table with code and symbol association (table for coding)
    Node *treeRoot; //root of a binary tree
    vector <bool> code; //binary vector
    /*
    * Associate a symbol with its code starting from the root
    * and passing through the tree
    */
    void symbolCodeAssociation(Node *root){
        if (root->left != NULL) {
            code.push_back(0); //put zero in the vector
            symbolCodeAssociation(root->left); //run the function for the left son
        }
        if (root->right!= NULL) {
            code.push_back(1); //put one in the vector
            symbolCodeAssociation(root->right); //run the function for the right son
        }
        if(root->left==NULL && root->right==NULL) { //if find a node with the letter
            symbolCodeTable[root->symbol]=code; //associate symbol with the code
        }
        code.pop_back(); //reducing code by one
    }
    /*
    * Build a code tree for characters of the text according to the Huffman's algorithm
    */
    void buildTree(const map <char, int> &symbolFrequencyAssociation){
        list <Node*> pointer; //list of pointers to the node
        
        //go through map and load nodes
        for (auto iter = symbolFrequencyAssociation.begin(); iter != symbolFrequencyAssociation.end(); ++iter) {
            Node *newNode = new Node; //create a new node
            newNode->symbol = iter->first;
            newNode->number = iter->second;
            pointer.push_back(newNode); //put pointer to the list
        }
        
        while (pointer.size()!=1) { //until map contains one element
            pointer.sort(MyCompare());
            
            Node *leftSon = pointer.front(); //take the first element
            pointer.pop_front(); //delete it
            Node *rightSon = pointer.front(); //take the second element
            pointer.pop_front(); //delete it
            Node *parent = new Node(leftSon, rightSon); //send to the constructor and find parent value
            pointer.push_back(parent); //put parent to the list
            
        }
        treeRoot = pointer.front(); //pointer to the root
        
        code.clear(); //free up memory
        symbolCodeTable.clear();
        symbolCodeAssociation(treeRoot); //creating a pair of character-code
    }
    vector<char> getSymbols(string path){
        vector<char> list;
        ifstream inputFile(path, ifstream::binary);
        
        while (!inputFile.eof())
            list.push_back(inputFile.get());
        
        inputFile.close();
        return list;
    }
    string getCode(vector<char> list){
        string code;
        int count = 0;
        char buf = 0;
        
        for (const auto &byte: list) {
            vector <bool> bin = symbolCodeTable[byte];//read code of symbol
            std::string temp = "";
            for (size_t i = 0; i < bin.size(); i++) {
                buf = buf | bin[i]<<(countByte - count - 1); //check if it's 0 or 1 and put it to byte
                count++;
                if(count == countByte){ //when byte is full, than zeroize counter, put this byte to the archive and zeroize buffer
                    count = 0;
                    code += buf; //wright code into file
                    buf = 0;
                }
            }
        }
        
        if (count != 0){
            code += buf; //put last, not full byte
        }
        
        return code;
    }
    void writeData(string path, int size, map <char,int> sFrequency, string scode){
        ofstream output(path, ios::binary);
        output.write(((char*)&size),sizeof(int));
        size_t sFsize = sFrequency.size();
        output.write(((char*)(&sFsize)), sizeof(size_t));
        for (auto iterator = sFrequency.begin(); iterator != sFrequency.end(); ++iterator) {
            output.write(((char*)&iterator->first),  sizeof(char));
            output.write(((char*)&iterator->second), sizeof(int));
        }
        
        output << scode;
        output.close();
    }
    
    tuple<int, map <char,int>, string> readData(string path){
        ifstream input(path, ios::in | ios::binary); // archive file
        int file_size;
        map <char,int> sFrequency;
        size_t size;
        string scode;
        
        input.read(((char*)&file_size), sizeof(int)); // read the file size
        input.read(((char*)&size), sizeof(size_t));
        // read symbols with the frequencies
        for (size_t i = 0; i < size; ++i) {
            char first;
            int second;
            input.read(((char*)&first), sizeof(char));
            input.read(((char*)&second), sizeof(int));
            sFrequency[first] = second;
        }
        //inputArchive.get();
        while (!input.eof()){
            scode += input.get();
        }
        
        input.close();
        return tuple<int, map <char,int>, string>(file_size, sFrequency, scode);
    }
    
    string getText(int file_size, string scode){
        string text;
        int size = file_size;
        Node *pointer = treeRoot; //pointer to the root
        int count=0;
        char byte;
        size_t i = 0;
        byte = scode[i]; //get byte
        
        while (i < scode.size()) {
            if (byte & (1 << (countByte - count - 1)))  //check if it's 0 or 1
                pointer = pointer->right;
            else
                pointer = pointer->left;
            if (pointer -> left == NULL && pointer -> right == NULL) {
                text += pointer -> symbol;
                if (--size == 0)
                    break; //all characters are encoded - stop decoding
                
                pointer = treeRoot;
            }
            count++;
            if (count == countByte){ //when byte is full, than zeroize counter and get new byte
                count = 0;
                byte = scode[i++];
            }
        }
        
        return text;
    }
    void writeDecData(string path, string text){
        ofstream outputFile(path, ios::out | ios::binary);// deachive file
        outputFile << text;
        outputFile.close();
    }
public:
    
    void encoding(string inputpath="file.txt", string outputpath="archive.huf"){
        vector<char> symbols = getSymbols(inputpath);
        map <char,int> sFrequency = Helper::getFrequency(symbols);
        buildTree(sFrequency);
        string scode = getCode(symbols);
        writeData(outputpath, symbols.size(), sFrequency, scode);
        
        delete treeRoot; //free up memory
        
    }
    void decoding(string inputpath="archive.huf", string outputpath="file_dec.txt"){
        auto data = readData(inputpath);
        int file_size = get<0>(data);
        auto sFrequency = get<1>(data);
        string scode = get<2>(data);
        buildTree(sFrequency);
        string text = getText(file_size, scode);
        writeDecData(outputpath, text);
        
        delete treeRoot; //free up memory
    }
};

int main (){
    Huffman huff;
    int choice;
    cout << "*-*-*-*-*-*-*-*_Huffman Archiver_*-*-*-*-*-*-* \n" << endl;
    cout << "    __ __ __ __\n   /           /|\n  /archive.huf/ |\n /__ __ __ __/ /|\n|__ __ __ __|/ /|\n|__ __ __ __|/ /\n|__ __ __ __|/\n" << endl;
    cout << "1.File archiving "<<endl;
    cout << "2.Unzip the file \n" << endl;
    cout << "Please, make your choice: ";
    cin >> choice;
    switch (choice) {
    case 1:
        huff.encoding();
        cout << "\nFile was saved with name: 'archive.huf' \n" << endl;
        cout << "Press Enter key to exit" << endl;
        break;
    case 2:
        huff.decoding();
        cout << "\nUnzipping successful \nFile was saved with name: 'archive.huf' \n" << endl;
        cout << "Press Enter key to exit" << endl;
        break;
    default:
        cout << "\nYou made a wrong choice! \n" << endl;
        cout << "Press Enter key to exit" << endl;
        break;
    }
    return 0;
}
