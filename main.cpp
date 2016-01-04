#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <fstream>

using namespace std;

class Node
{
public:
    int frequency; //frequency of sign in the file
    char symbol; //value (only for leaves)
    Node *left, *right; // pointers to the left and right son

    Node(){left=right=NULL;} // constructor overload

    Node (Node *Left, Node *Right){ // constructor takes a reference to the left and right son
        left = Left;
        right = Right;
        frequency = Left->frequency + Right->frequency; // the sum of two numbers
    }
    ~Node(){ // destructor
        delete left;
        delete right;
    }
};

struct MyCompare{ // for sorting elements in list
    bool operator()(Node* left, Node* right){ //parameters overload
        return left->frequency < right->frequency;
    }
};

//global variables
map <char, vector <bool> > symbolCodeTable;//table with code and symbol association (table for coding)
Node *treeRoot; // root of a binary tree

vector <bool> code; // vector of zeros and ones (only for symbolCodeAssociation)

/* Associate a symbol with its code starting from the root
 * and passing through the tree
 */

void symbolCodeAssociation(Node *root){
    if (root->left!=NULL){
        code.push_back(0); // put zero in the vector
        symbolCodeAssociation(root->left); // run the function for the left son
    }
    if (root->right!= NULL){
        code.push_back(1);// put one in the vector
        symbolCodeAssociation(root->right);// run the function for the right son
    }
    if(root->left==NULL && root->right==NULL){ // if find a node with the letter
        symbolCodeTable[root->symbol]=code; // associate symbol with the code
    }
    code.pop_back();// reducing code by one

}

void buildTree(const map <char, int> &symbolFrequencyAssociation)
{
    list <Node*> pointer; // list of pointers to the node

    for (map <char, int>::const_iterator iter = symbolFrequencyAssociation.begin(); iter != symbolFrequencyAssociation.end(); ++iter){ // go through map and load nodes
        Node *newNode = new Node; // create a new node
        newNode->symbol = iter->first;
        newNode->frequency = iter->second;
        pointer.push_back(newNode); // put pointer to the list
    }

    while (pointer.size()!=1){ // while there will be one element
        pointer.sort(MyCompare());

        Node *leftSon = pointer.front(); // take the first element
        pointer.pop_front(); // delete it
        Node *rightSon = pointer.front(); // take the second element
        pointer.pop_front(); // delete it

        Node *parent = new Node(leftSon, rightSon);// send to the constructor and find parent value
        pointer.push_back(parent);// put parent to the list

    }
    treeRoot = pointer.front(); // pointer to the root

    code.clear();
    symbolCodeTable.clear();
    symbolCodeAssociation(treeRoot); // creating a pair of character-code
}
//function prototypes
void archive(); // encoding function
void dearchive(); // decoding function

int main ()
{
    int choice;
    cout<<"*-*-*-*-*-*-*-*_Huffman Archiver_*-*-*-*-*-*-* \n"<<endl;
    cout<<"    __ __ __ __\n   /           /|\n  /archive.huf/ |\n /__ __ __ __/ /|\n|__ __ __ __|/ /|\n|__ __ __ __|/ /\n|__ __ __ __|/\n"<<endl;
    cout<<"1.File archiving "<<endl;
    cout<<"2.Unzip the file \n"<<endl;
    cout<<"Please, make your choice: ";
    cin >> choice;
    switch (choice) {
    case 1:
        archive();
        cout << "\nFile was saved with name: 'archive.huf' \n" << endl;
        cout << "Press Enter key to exit" << endl;
        break;
    case 2:
        dearchive();
        cout << "\nUnzipping successful \nFile was saved with name: 'archive.huf' \n" << endl;
        cout << "Press Enter key to exit" << endl;
        break;
    default:
        cout<<"\nYou made a wrong choice! \n"<<endl;
        cout << "Press Enter key to exit" << endl;
        break;
    }

    system("pause");
    return 0;
}

void archive()
{
    ifstream inputFile("file.txt", ifstream::binary); //input file

    map <char,int> symbolFrequencyAssociation;
    int frequency;
    int file_size = 0;
    while((frequency = inputFile.get()) != EOF) // until not the end of file, get one byte
    {
        symbolFrequencyAssociation[frequency]++;
        ++file_size;
    }

    buildTree(symbolFrequencyAssociation);

    inputFile.clear();
    inputFile.seekg(0); // move the pointer to the beginning of the file (as it is at the moment at the end)

    ofstream outputArchive("archive.huf", ios::binary);

    outputArchive.write(reinterpret_cast<char *>(&file_size), sizeof(file_size)); // write the file size
    char size = symbolFrequencyAssociation.size();
    outputArchive.write(reinterpret_cast<char *>(&size), sizeof(size)); // the number of symbols
    // write symbols with the frequencies
    for (map <char, int>::iterator iterator = symbolFrequencyAssociation.begin(); iterator != symbolFrequencyAssociation.end(); ++iterator)
    {
        char first = iterator->first;
        int second = iterator->second;
        outputArchive.write(reinterpret_cast<char *>(&first), sizeof(char)); // typecast to char *
        outputArchive.write(reinterpret_cast<char *>(&second), sizeof(int));
    }

    int count=0; // counter
    char buf=0;
    while (true)
    {
        char byte = inputFile.get(); // get one byte
        if (inputFile.eof()){
        break; // if end of file - out of cycle
        }
        vector <bool> bin = symbolCodeTable[byte];// read code of symbol
        for (int i=0; i<bin.size(); i++){
            buf = buf | bin[i]<<(7-count); // check if it's 0 or 1 and put it to byte
            count++;
            if(count==8){
                count = 0;
                outputArchive<<buf; // wright code into file
                buf=0;
            }
        }
    }
    if (count != 0) outputArchive << buf; // put last not full byte
    inputFile.close();
    outputArchive.close();

    delete treeRoot; // free up memory
}




void dearchive(){
    ifstream inputArchive("archive.huf", ios::in | ios::binary); // archive file
    ofstream outputFile("file_dec.txt", ios::out | ios::binary);// deachive file

    int file_size;
    inputArchive.read(reinterpret_cast<char *>(&file_size), sizeof(file_size)); // read the file size

    map <char,int> symbolFrequencyAssociation;
    char size;
    inputArchive.read(reinterpret_cast<char *>(&size), sizeof(size)); // typecast to char *
    // read symbols with the frequencies
    for (int i = 0; i < size; ++i)
    {
        char first;
        int second;
        inputArchive.read(reinterpret_cast<char *>(&first), sizeof(char));
        inputArchive.read(reinterpret_cast<char *>(&second), sizeof(int));
        symbolFrequencyAssociation[first] = second;
    }

    buildTree(symbolFrequencyAssociation);

    Node *pointer = treeRoot; // pointer to the root
    int count=0;
    char byte;
    byte = inputArchive.get(); // get byte
    while(!inputArchive.eof()){
        if (byte & (1 << (7-count))) { // check if it's 0 or 1
            pointer = pointer->right;
        }else{
            pointer = pointer->left;
        }
        if (pointer->left==NULL && pointer->right==NULL) {
            outputFile << pointer->symbol;

            if(--file_size == 0){
                break; // all characters are encoded - stop decoding
            }
            pointer = treeRoot;
        }
        count++;
        if (count==8){ // when 8 - get new byte
            count=0;
            byte = inputArchive.get();
        }
    }
    inputArchive.close();
    outputFile.close();

    delete treeRoot; // free up memory
}
