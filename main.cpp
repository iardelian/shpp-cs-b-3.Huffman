#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <fstream>

using namespace std;

class Node{
public:
    int a; //number
    char c; //symbol
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
    bool operator()(Node* l,Node* r) { //parameters overload
        return l->a < r->a;
    }
};

//function prototypes
void archive();
void dearchive();

//global variables
vector<bool> code; // vector of zeros and ones
map<char, vector<bool> > table;// code and symbol association
Node *root; // root of a binary tree

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
    return 0;
}
void archive(){
 ifstream f("file.txt", ios::in | ios::binary); //input file

 map <char,int> m;

 while(!f.eof()){ // until not the end of file
     char c = f.get(); // get one byte
     m[c]++;
 }

 list <Node*> t; // list of pointers to the node

 map <char,int>::iterator iter;
 for (iter = m.begin(); iter != m.end(); ++iter){ // go through map and load nodes
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
     t.push_back(parent);// send parent to the constructor

 }
 root = t.front(); // pointer to the root

 BuildTable(root); // creating a pair of character-code

 f.clear(); f.seekg(0); // move the pointer to the beginning of the file (as it is at the moment at the end)

 ofstream g("archive.huf", ios::out | ios::binary);
 int count=0; // counter
 char buf=0;
 while(!f.eof()){ // until not the end of file
     char c = f.get(); // get one byte
     vector<bool> x = table[c];
     for (int n=0; n < x.size(); n++){
         buf = buf | x[n]<<(7-count); // vector of 0 and 1 put to byte
         count++;
         if(count==8){
             count = 0;
             g<<buf;
             buf=0;
         }
     }
 }
 f.close(); // close files
 g.close();
}

void dearchive(){
 ifstream A("archive.huf", ios::in | ios::binary);
 ofstream O("outfile.txt", ios::out | ios::binary);
 Node *p = root; // root pointer
 int count=0;
 char byte;
 byte = A.get(); // get byte
 while(!A.eof()){
     bool b = byte & (1 << (7-count));
     if (b) p=p->right; else p=p->left;
     if(p->left==NULL && p->right==NULL){
         p = root;
         O<<(p->c);
     }
     count++;
     if (count==8){ // when 8 - get new byte
         count=0;
         byte = A.get();
     }
 }
 A.close();
 O.close(); // close files
}




