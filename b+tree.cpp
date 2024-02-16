#include <iostream>
#include <cstdlib>
using namespace std;

class node {
public:
    int *keys;
    node **child;
    node *next;
    int n;
    bool isleaf;
    node(int order, bool leaf = true) {
        keys = new int[order];
        child = new node *[order + 1];
        next = NULL;
        n = 0;
        isleaf = leaf;
    }
    ~node() {
        delete[] keys;
        delete[] child;
    }
};

class bplustree {
    int order;
    node *root, *first;

public:
    bplustree(int t) {
        order = t;
        root = NULL;
        first = NULL;
    }

    void insert(int key) {
        if (root == NULL) {
            root = new node(order, true);
            root->keys[0] = key;
            root->n = 1;
            first = root;
        } else {
            if (root->n == order) {
                node *newroot = new node(order, false);
                newroot->child[0] = root;
                splitChild(newroot, 0);
                root = newroot;
            }
            insertnon_full(root, key);
        }
    }
    bool search(int key) {
        return searchKey(root, key);
    }

    void remove(int key) {
        if (root == nullptr) {
            cout << "Tree is empty. Cannot delete key " << key << "." << endl;
            return;
        }

        removeKey(root, key);

        if (root->n == 0 && !root->isleaf) {
            node *temp = root;
            root = root->child[0];
            delete temp;
        }
    }
    void printTree() {
        cout<<"Tree: ";
        if (root != nullptr) {
            inOrderTraversal(root);
            cout << endl;
        }
    }

    void printLeaves() {
        cout<<"Leaves: ";
        node *current = first;
        while (current != nullptr) {
            for (int i = 0; i < current->n; i++) {
                std::cout << current->keys[i] << " ";
            }
            current = current->next;

            
            if (current != nullptr) {
                std::cout << "| ";
            }
        }
        std::cout << std::endl;
    }
private:
    void insertnon_full(node *root, int key) {
        int i = root->n - 1;
        if (root->isleaf) {
            while (i >= 0 && key < root->keys[i]) {
                root->keys[i + 1] = root->keys[i];
                i--;
            }
            root->keys[i + 1] = key;
            root->n++;
        } else {
            while (i >= 0 && key < root->keys[i]) {
                i--;
            }
            i++;
            if (root->child[i]->n == order) {
                splitChild(root, i);
                if (key > root->keys[i]) {
                    i++;
                }
            }
            insertnon_full(root->child[i], key);
        }
    }

    void splitChild(node *parent, int index) {
        node *child = parent->child[index];
        node *newChild = new node(order, child->isleaf);
        parent->n++;

        for (int i = parent->n - 1; i > index; i--) {
            parent->keys[i] = parent->keys[i - 1];
        }
        int newn = order/2;

        parent->keys[index] = child->keys[newn];

        for (int i = parent->n; i > index + 1; i--) {
            parent->child[i] = parent->child[i - 1];
        }
        parent->child[index + 1] = newChild;
        

        for (int i = 0; i <= (order - newn) ; i++) {
            newChild->keys[i] = child->keys[i + newn];
        }
        child->n= newn;
        newChild->n = order -newn;

        if (!child->isleaf) {
            for (int i = 0; i <= order / 2; i++) {
                newChild->child[i] = child->child[i + newn];
            }
        }

        if (child->isleaf) {
            newChild->next = child->next;
            child->next = newChild;
        }
    }
    
    bool searchKey(node *root, int key) {
        if (root == nullptr) {
            return false;
        }

        int i = 0;
        while (i < root->n && key > root->keys[i]) {
            i++;
        }

        if (i < root->n && key == root->keys[i]) {
            return true; 
        } else if (root->isleaf) {
            return false; 
        } else {
            return searchKey(root->child[i], key); 
        }
    }

    void removeKey(node *current, int key) {
        int index = 0;
        while (index < current->n && key > current->keys[index]) {
            index++;
        }

        if (current->isleaf) {
            if (index == current->n || current->keys[index] != key) {
                cout << "Key " << key << " not found in the tree." << endl;
                return;
            }

            for (int i = index; i < current->n - 1; i++) {
                current->keys[i] = current->keys[i + 1];
            }
            current->n--;
        } else {
            bool keyFound = (index < current->n && current->keys[index] == key);
            if (keyFound) {
                if (!current->child[index]->isleaf) {
                    int predecessor = getPredecessor(current, index);
                    removeKey(current->child[index], predecessor);
                    current->keys[index] = predecessor;
                } else {
                    int successor = getSuccessor(current, index);
                    removeKey(current->child[index + 1], successor);
                    current->keys[index] = successor;
                }
            } else {
                removeKey(current->child[index], key);
            }
        }

        if (!current->isleaf && current->child[index]->n < (order + 1) / 2 - 1) {
            handleUnderflow(current, index);
        }

    }

    int getPredecessor(node *current, int index) {
        node *temp = current->child[index];
        while (!temp->isleaf) {
            temp = temp->child[temp->n];
        }
        return temp->keys[temp->n - 1];
    }

    int getSuccessor(node *current, int index) {
        node *temp = current->child[index + 1];
        while (!temp->isleaf) {
            temp = temp->child[0];
        }
        return temp->keys[0];
    }

    void handleUnderflow(node *parent, int index) {
        node *child = parent->child[index];
        node *sibling;

        if (index > 0 && parent->child[index - 1]->n > (order - 1) / 2) {
            sibling = parent->child[index - 1];
            borrowFromLeft(child, sibling, parent, index - 1);
        } else if (index < parent->n && parent->child[index + 1]->n > (order - 1) / 2) {
            sibling = parent->child[index + 1];
            borrowFromRight(child, sibling, parent, index);
        } else if (index > 0) {
            sibling = parent->child[index - 1];
            mergeNodes(sibling, child, parent, index - 1);
        } else {
            sibling = parent->child[index + 1];
            mergeNodes(child, sibling, parent, index);
        }
    }

    void borrowFromLeft(node *child, node *leftSibling, node *parent, int leftSiblingIndex) {
        for (int i = child->n; i > 0; i--) {
            child->keys[i] = child->keys[i - 1];
        }
        if (!child->isleaf) {
            for (int i = child->n + 1; i > 0; i--) {
                child->child[i] = child->child[i - 1];
            }
        }

        child->keys[0] = parent->keys[leftSiblingIndex];
        parent->keys[leftSiblingIndex] = leftSibling->keys[leftSibling->n - 1];

        if (!child->isleaf) {
            child->child[0] = leftSibling->child[leftSibling->n];
        }

        child->n++;
        leftSibling->n--;
    }

    void borrowFromRight(node *child, node *rightSibling, node *parent, int rightSiblingIndex) {
        child->keys[child->n] = parent->keys[rightSiblingIndex];
        parent->keys[rightSiblingIndex] = rightSibling->keys[0];

        if (!child->isleaf) {
            child->child[child->n + 1] = rightSibling->child[0];
        }

        for (int i = 0; i < rightSibling->n - 1; i++) {
            rightSibling->keys[i] = rightSibling->keys[i + 1];
        }

        if (!rightSibling->isleaf) {
            for (int i = 0; i <= rightSibling->n; i++) {
                rightSibling->child[i] = rightSibling->child[i + 1];
            }
        }

        child->n++;
        rightSibling->n--;
    }

    void mergeNodes(node *leftNode, node *rightNode, node *parent, int parentIndex) {
        leftNode->keys[leftNode->n] = parent->keys[parentIndex];

        for (int i = 0; i < rightNode->n; i++) {
            leftNode->keys[leftNode->n + 1 + i] = rightNode->keys[i];
        }

        if (!leftNode->isleaf) {
            for (int i = 0; i <= rightNode->n; i++) {
                leftNode->child[leftNode->n + 1 + i] = rightNode->child[i];
            }
        }

        // Update the parent's key
        for (int i = parentIndex; i < parent->n - 1; i++) {
            parent->keys[i] = parent->keys[i + 1];
            parent->child[i + 1] = parent->child[i + 2];
        }

        leftNode->n += (1 + rightNode->n);
        parent->n--;
        delete rightNode;
    }


    void inOrderTraversal(node *node) {
        if (node != nullptr) {
            int i;
            for (i = 0; i < node->n; i++) {
                if (!node->isleaf) {
                    inOrderTraversal(node->child[i]);
                }
                std::cout << node->keys[i] << " ";
            }
            if (!node->isleaf) {
                inOrderTraversal(node->child[i]);
            }
        }
    }

    
};
void menu(){
    cout<<"********MENU********"<<endl;
    cout<<"Enter 1 to insert"<<endl;
    cout<<"Enter 2 to delete"<<endl;
    cout<<"Enter 3 to search"<<endl;
    cout<<"Enter 4 to print tree"<<endl;
    cout<<"Enter 5 to print leaves"<<endl;
    cout<<"Enter 6 to exit"<<endl;
    cout<<"Choice: ";
}
int main(){
    cout<<"Choose order of your b+tree: ";
    int order;cin>>order;
    order=order-1;
    bplustree b(order);
    int choice;

    //initializing a dummy tree
    b.insert(10);
    b.printTree();
    b.printLeaves();

    b.insert(20);
    b.printTree();
    b.printLeaves();

    b.insert(30);
    b.printTree();
    b.printLeaves();

    b.insert(40);
    b.printTree();
    b.printLeaves();

    b.insert(50);
    b.printTree();
    b.printLeaves();

    b.insert(60);
    b.printTree();
    b.printLeaves();

    b.insert(15);
    b.printTree();
    b.printLeaves();

    b.insert(25);
    b.printTree();
    b.printLeaves();

    b.insert(5);
    b.printTree();
    b.printLeaves();

    b.insert(45);
    b.printTree();
    b.printLeaves();

    b.insert(35);
    b.printTree();
    b.printLeaves();

    b.insert(55);
    b.printTree();
    b.printLeaves();

    b.insert(65);
    b.printTree();
    b.printLeaves();

    b.insert(37);
    b.printTree();
    b.printLeaves();

    b.insert(29);
    b.printTree();
    b.printLeaves();

    b.insert(13);
    b.printTree();
    b.printLeaves();

    
    cout<<"Dummy B+ Tree: "<<endl;
    b.printTree();
    b.printLeaves();

    while(true){
        menu();
        cin>>choice;
        switch(choice){
            case 1:{
                int key;
                cout<<"Enter key: ";
                cin>>key;
                b.insert(key);
                cout<<"----> inserted successfully <----"<<endl;
                b.printTree();
                b.printLeaves();
                break;
            }
            case 2:{
                int key;
                cout<<"Enter key: ";
                cin>>key;
                bool found = b.search(key);
                b.remove(key);
                b.printTree();
                b.printLeaves();
                if(!b.search(key) && found){
                    cout << "Key " << key << " removed from the B+ tree." << endl;
                }
                break;
            }
            case 3:{
                int key;
                cout<<"Enter key: ";
                cin>>key;
                if(b.search(key)){
                    cout << "Key " << key << " found in the B+ tree." << endl;
                }
                else{
                    cout << "Key " << key << " not found in the B+ tree." << endl;
                }
                break;
            }
            case 4:{
                b.printTree();
                break;
            }
            case 5:{
                b.printLeaves();
                break;
            }
            case 6:{
                cout<<"Exiting..."<<endl;
                exit(0);
            }
            default:
                cout<<"Wrong choice"<<endl;
        }
    }
    return 0;
}