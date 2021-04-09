#include <sstream>
#include <string>
#include <iostream>
#include <type_traits>
using namespace std;

#ifndef ILIST_H
#define ILIST_H

template <class T>
class IList
{
public:
    virtual void add(const T& element) = 0;
    virtual void add(int index, const T& element) = 0;
    virtual T removeAt(int index) = 0;
    virtual bool removeItem(const T& item) = 0;
    virtual bool empty() = 0;
    virtual int size() = 0;
    virtual void clear() = 0;
    virtual T get(int index) = 0;
    virtual void set(int index, const T& element) = 0;
    virtual int indexOf(const T& item) = 0;
    virtual bool contains(const T& item) = 0;
    virtual string toString() = 0;
};

#endif
template <class T>
class FragmentLinkedList : IList<T>
{
public:
    class Node;
    class Iterator;

protected:
    Node** fragmentPointers;
    int fragmentMaxSize;
    int count;

public:
    FragmentLinkedList(int fragmentMaxSize = 5)
    {
        this->count = 0;
        this->fragmentMaxSize = fragmentMaxSize;
        this->fragmentPointers = new Node * [2];
        this->fragmentPointers[0] = NULL;
        this->fragmentPointers[1] = NULL;
    }
    ~FragmentLinkedList()
    {
        clear();
    }
    void add(const T& element)
    {
        if (empty())
        {
            this->fragmentPointers[0] = new Node(element, NULL, NULL);
            this->fragmentPointers[1] = this->fragmentPointers[0];
            this->count = 1;
        }
        else
        {
            this->fragmentPointers[0]->prev = new Node(element, this->fragmentPointers[0], NULL);
            this->fragmentPointers[0] = this->fragmentPointers[0]->prev; this->count++;
            resize(this->count);
        }
    }
    void resize(int Count)
    {
        Node* temp = this->fragmentPointers[0]; delete[]this->fragmentPointers;
        if (Count % this->fragmentMaxSize == 0) this->fragmentPointers = new Node * [(Count / this->fragmentMaxSize) + 1];
        else this->fragmentPointers = new Node * [(Count / this->fragmentMaxSize) + 2];
        this->fragmentPointers[0] = temp;
        int idx = 0; int num = 0;
        while (temp->next != nullptr) {
            if (num % this->fragmentMaxSize == 0)
            {
                this->fragmentPointers[idx] = temp;
                idx++;
            }
            temp = temp->next; num++;
            if (temp->next == nullptr)
            {
                if (num % this->fragmentMaxSize == 0)
                {
                    this->fragmentPointers[idx] = temp;
                    idx++;
                }
                break;
            }
        }
        if (idx == 0) idx += 1;
        this->fragmentPointers[idx] = temp;
        temp = nullptr;
    }
    void add(int index, const T& element)
    {
        if (index == 0)
        {
            this->add(element);  return;
        }
        else if (index < 0 || index > this->count || empty()) return;
        else
        {
            Node* temp = this->fragmentPointers[index / this->fragmentMaxSize];
            if (index < this->count)
            {
                for (int i = 0; i < index % this->fragmentMaxSize; i++) temp = temp->next;
                temp->prev->next = new Node(element, temp, temp->prev);
                temp->prev = temp->prev->next; this->count++; temp = nullptr;
            }
            else if (index == this->count) {
                for (int i = 0; i < index % this->fragmentMaxSize - 1; i++) temp = temp->next;
                temp->next = new Node(element, NULL, temp); this->count++; temp = nullptr;
            }
            resize(this->count);
        }
    }
    T removeAt(int index)
    {
        T ret;
        if (empty()) {
            throw "The list is empty!";
        }
        else if (index < 0 || index >= this->count || empty()) {
            throw "Invalid index";
        }
        else
        {
            if (index == 0) {
                ret = this->fragmentPointers[0]->data;
                this->fragmentPointers[0] = this->fragmentPointers[0]->next;
                delete this->fragmentPointers[0]->prev; this->count--;
            }
            else if (index == this->count - 1) {
                if (this->count % this->fragmentMaxSize == 0)
                {
                    ret = this->fragmentPointers[this->count / this->fragmentMaxSize]->data;
                    Node* temp = this->fragmentPointers[this->count / this->fragmentMaxSize]->prev;
                    temp->next = nullptr;
                    delete this->fragmentPointers[this->count / this->fragmentMaxSize];
                }
                else
                {
                    ret = this->fragmentPointers[this->count / this->fragmentMaxSize + 1]->data;
                    Node* temp = this->fragmentPointers[this->count / this->fragmentMaxSize + 1]->prev;
                    temp->next = nullptr;
                    delete this->fragmentPointers[this->count / this->fragmentMaxSize + 1];
                }
                this->count--;
            }
            else {
                Node* temp = this->fragmentPointers[index / this->fragmentMaxSize];
                for (int i = 0; i < index % this->fragmentMaxSize; i++) temp = temp->next;
                temp->next->prev = temp->prev;
                temp->prev->next = temp->next;
                ret = temp->data;
                delete temp; this->count--;
            }
            resize(this->count);
            return ret;
        }
    }
    bool removeItem(const T& item)
    {
        if (empty()) return false;
        else
        {
            Node* temp = this->fragmentPointers[0];
            if (temp->data == item) {
                this->fragmentPointers[0] = this->fragmentPointers[0]->next;
                delete this->fragmentPointers[0]->prev; this->count--;
                resize(this->count);
                return true;
            }
            else {
                while (temp->next != nullptr)
                {
                    if (temp->data == item)
                    {
                        temp->next->prev = temp->prev;
                        temp->prev->next = temp->next;
                        delete temp; this->count--;
                        resize(this->count);
                        return true;
                    }
                    temp = temp->next;
                }
                if (temp->data == item) {
                    temp->prev->next = temp->next;
                    delete temp;
                    this->count--;
                    resize(this->count);
                    return true;
                }
            }
            return false;
        }
    }
    bool empty()
    {
        if (this->count == 0) return true;
        return false;
    }
    int size()
    {
        return this->count;
    }
    void clear()
    {
        if (empty()) return;
        Node* temp = this->fragmentPointers[0];
        while (temp)
        {
            Node* camp = temp;
            temp = temp->next;
            delete camp;
            camp = nullptr;
        }
        delete[]this->fragmentPointers;
        this->fragmentPointers = nullptr;
        this->count = 0;
    }
    T get(int index)
    {
        if (index < 0 || index >= this->count) {
            throw "Invalid index!"; 
        }
        Node* temp = this->fragmentPointers[index / this->fragmentMaxSize];
        for (int i = 0; i < index % this->fragmentMaxSize; i++) temp = temp->next;
        return temp->data;
    }
    void set(int index, const T& element)
    {
        if (index < 0 || index >= this->count) {
            return;
        }
        Node* temp = this->fragmentPointers[index / this->fragmentMaxSize];
        for (int i = 0; i < index % this->fragmentMaxSize; i++) temp = temp->next;
        temp->data = element;
    }
    int indexOf(const T& item)
    {
        Node* temp = this->fragmentPointers[0]; int idx = 0;
        while (temp->next != nullptr)
        {
            if (temp->data == item) return idx;
            temp = temp->next; idx++;
            if (temp->next == nullptr) {
                if (temp->data == item) return idx;
            }
        }
        return -1;
    }
    bool contains(const T& item) //Done
    {
        Node* temp = this->fragmentPointers[0];
        while (temp->next != nullptr)
        {
            if (temp->data == item) return true;
            temp = temp->next;
            if (temp->next == nullptr) {
                if (temp->data == item) return true;
            }
        }
        return false;
    }
    string toString();
    Iterator begin(int index = 0)
    {
        return Iterator(index, this, true);
    }
    Iterator end(int index = -1)
    {
        if (index == -1)
        {
            if (this->count % this->fragmentMaxSize == 0) return Iterator(this->count / this->fragmentMaxSize, this, false);
            else return Iterator(this->count / this->fragmentMaxSize + 1, this, false);
        }
        else
        {
            return Iterator(index, this, false);
        }
    }

public:
    class Node
    {
    private:
        T data;
        Node* next;
        Node* prev;
        friend class FragmentLinkedList<T>;

    public:
        Node()
        {
            next = 0;
            prev = 0;
        }
        Node(Node* next, Node* prev)
        {
            this->next = next;
            this->prev = prev;
        }
        Node(T data, Node* next, Node* prev)
        {
            this->data = data;
            this->next = next;
            this->prev = prev;
        }
    };

    class Iterator
    {
    private:
        FragmentLinkedList<T>* pList;
        Node* pNode;
        int index;
    public:
        Iterator(FragmentLinkedList<T>* pList = 0, bool begin = true)
        {
            this->index = 0;
            this->pList = pList;
            if (this->pList->fragmentPointers[0] == nullptr) return;
            if (begin)  this->pNode = pList->fragmentPointers[0];
            else if (!begin)
            {
                if (this->count % this->fragmentMaxSize == 0) this->pNode = pList->fragmentPointers[pList->count / pList->fragmentMaxSize]->next;
                else this->pNode = pList->fragmentPointers[pList->count / pList->fragmentMaxSize + 1]->next;
            }
        }
        Iterator(int fragmentIndex = 0, FragmentLinkedList<T>* pList = 0, bool begin = true)
        {
            this->pList = pList;
            if (this->pList->fragmentPointers[0] == nullptr) return;
            if (this->pList->fragmentPointers[fragmentIndex]->next == nullptr) this->index = this->pList->count;
            else this->index = this->pList->fragmentMaxSize * fragmentIndex;
            if (begin)  this->pNode = this->pList->fragmentPointers[fragmentIndex];
            else if (!begin)
            {
                this->pNode = this->pList->fragmentPointers[fragmentIndex];
                while (this->pNode != nullptr && this->pNode != this->pList->fragmentPointers[fragmentIndex + 1])
                {
                    this->pNode = this->pNode->next;
                }
            }
        }
        Iterator& operator=(const Iterator& iterator)
        {
            this->pNode = iterator.pNode;
            this->index = iterator.index;
            return *this;
        }
        T& operator*()
        {
            if (!(this->pNode))
            {
                throw std::out_of_range("Segmentation fault!");
            }
            return this->pNode->data;
        }
        bool operator!=(const Iterator& iterator)
        {
            return !(this->pNode == iterator.pNode);
        }
        void remove()
        {
            int temp = this->index;
            this->index -= 1;
            if (this->index < 0)
            {
                this->index += 1;
                this->pNode = this->pNode->next;
            }
            else
            {
                this->pNode = this->pNode->prev;
            }
            try
            {
                this->pList->removeAt(temp);
                this->pList->resize(this->pList->count);
            }
            catch (const char* msg)
            {
                cerr << msg << endl;
            }
        }
        void set(const T& element)
        {
            Node* temp = this->pList->fragmentPointers[this->index / this->pList->fragmentMaxSize];
            for (int i = 0; i < this->index % this->pList->fragmentMaxSize; i++) temp = temp->next;
            temp->data = element;
        }
        Iterator& operator++()
        {
            if ((this->pNode))
            {
                this->pNode = this->pNode->next;
                this->index += 1;
            }
            return *this;
        }
        Iterator operator++(int)
        {
            Iterator temp = *this;
            if ((this->pNode))
            {
                this->pNode = this->pNode->next;
                this->index += 1;
            }
            return temp;
        }
    };
};

template <class T>
string FragmentLinkedList<T>::toString()
{
    stringstream ss;
    ss << "[";
    if (!this->empty())
    {
        Node* ptr = this->fragmentPointers[0];
        while (ptr->next != nullptr)
        {
            ss << ptr->data << ", ";
            ptr = ptr->next;
            if (ptr->next == nullptr) ss << ptr->data;
        }
    }
    ss << "]";

    return ss.str();
}

int main()
{
    FragmentLinkedList<int> list;
    cout << list.size();
    return 0;
}