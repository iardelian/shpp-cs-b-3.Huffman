#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <fstream>


using namespace std;

class Node
{
public:
    int a; //number
    char c; //symbol (only for leaves)
    Node *left, *right; // pointers to the left and right son

    Node(){left=right=NULL;} // constructor overload

    Node (Node *L, Node *R){ // constructor takes a reference to the left and right son
        left = L;
        right = R;
        a = L->a + R->a; // the sum of two numbers
    }
    ~Node(){ // destructor
        delete left;
        delete right;
    }
};

struct MyCompare{
    bool operator()(Node* l, Node* r){ //parameters overload
        return l->a < r->a;
    }
};

//global variables
map<char, vector<bool> > table;//code with symbol association (table for coding)
Node *root; // root of a binary tree

vector<bool> code; // vector of zeros and ones (only for BuildTable)

/* Associate a symbol with its code starting from the root
 * and passing through the tree
 */

void BuildTable(Node *root1){
    if (root1->left!=NULL){
        code.push_back(0); // put zero in the vector
        BuildTable(root1->left); // run the function for the left son
    }
    if (root1->right!= NULL){
        code.push_back(1);// put one in the vector
        BuildTable(root1->right);// run the function for the right son
    }
    if(root1->left==NULL && root1->right==NULL){ // if find a node with the letter
        table[root1->c]=code; // associate symbol with the code
    }
    code.pop_back();// reducing code by one

}

void Initialize(const map <char, int> &m)
{
    list <Node*> t; // list of pointers to the node

    for (map <char, int>::const_iterator iter = m.begin(); iter != m.end(); ++iter){ // go through map and load nodes
        Node *p = new Node; // create a new node
        p->c = iter->first;
        p->a = iter->second;
        t.push_back(p); // add a pointer to the list
    }

    while (t.size()!=1){ // while there will be one element
        t.sort(MyCompare());

        Node *SonL = t.front(); // take the first element
        t.pop_front(); // delete it
        Node *SonR = t.front(); // take the second element
        t.pop_front(); // delete it

        Node *parent = new Node(SonL,SonR);// send to the constructor
        t.push_back(parent);// put parent to the constructor

    }
    root = t.front(); // pointer to the root

    code.clear();
    table.clear();
    BuildTable(root); // creating a pair of character-code
}
//function prototypes
void archive(); // encoding function
void dearchive(); // decoding function

int main ()
{
    int choice;
    cout<<"*-*-*-*-*-*-*-*_Huffman Archiver_*-*-*-*-*-*-* \n"<<endl;
    cout<<"    __ __ __ __\n   /           /|\n  /   *.huf   / |\n /__ __ __ __/ /|\n|__ __ __ __|/ /|\n|__ __ __ __|/ /\n|__ __ __ __|/\n"<<endl;
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
    ifstream f("file.txt", ifstream::binary); //input file

    map <char,int> m;
    int c, file_size = 0;
    while((c = f.get()) != EOF) // until not the end of file get one byte
    {
        m[c]++;
        ++file_size;
    }

    Initialize(m);

    f.clear(); f.seekg(0); // move the pointer to the beginning of the file (as it is at the moment at the end)

    ofstream g("archive.huf", ios::binary);

    g.write(reinterpret_cast<char *>(&file_size), sizeof(file_size)); // write the file size
    unsigned short sz = m.size();
    g.write(reinterpret_cast<char *>(&sz), sizeof(sz)); // the number of symbols
    // write symbols with the frequencies
    for (map<char, int>::iterator iter = m.begin(); iter != m.end(); ++iter)
    {
        char fr = iter->first;
        int sc = iter->second;
        g.write(reinterpret_cast<char *>(&fr), sizeof(char)); // typecast to char *
        g.write(reinterpret_cast<char *>(&sc), sizeof(int));
    }

    int count=0; // counter
    char buf=0;
    while (true)
    {
        char c = f.get(); // get one byte
        if (f.eof()) break; // if end of file - out of cycle
        vector<bool> x = table[c];
        for (int n=0; n<x.size(); n++){
            buf = buf | x[n]<<(7-count); // vector of 0 and 1 put to byte
            count++;
            if(count==8){
                count = 0;
                g<<buf;
                buf=0;
            }
        }
    }
    if (count != 0) g << buf; // put last not full byte
    f.close();
    g.close();

    delete root; // free up memory
}

void dearchive(){
    ifstream A("archive.huf", ios::in | ios::binary); // Archive file
    ofstream O("file_dec.txt", ios::out | ios::binary);// Output file

    int file_size;
    A.read(reinterpret_cast<char *>(&file_size), sizeof(file_size)); // read the file size

    map <char,int> m;
    unsigned short sz;
    A.read(reinterpret_cast<char *>(&sz), sizeof(sz)); // количество символов
    // read symbols with the frequencies
    for (int i = 0; i < sz; ++i)
    {
        char fr;
        int sc;
        A.read(reinterpret_cast<char *>(&fr), sizeof(char));
        A.read(reinterpret_cast<char *>(&sc), sizeof(int));
        m[fr] = sc;
    }

    Initialize(m);

    Node *p = root; // pointer to the root
    int count=0;
    char byte;
    byte = A.get(); // get byte
    while(!A.eof()){
        bool b = byte & (1 << (7-count));
        if (b) p=p->right; else p=p->left;
        if(p->left==NULL && p->right==NULL){
            O << p->c;
            if(--file_size == 0){
            break; // all characters are encoded - stop decoding
            }
            p = root;
        }
        count++;
        if (count==8){ // when 8 - get new byte
            count=0;
            byte = A.get();
        }
    }
    A.close();
    O.close();

    delete root; // free up memory
}
