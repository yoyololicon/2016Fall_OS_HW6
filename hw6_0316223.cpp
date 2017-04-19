#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
using namespace std;

class Item
{
public:
    Item(string key);
    string value;
    Item *next, *pre;
};
Item::Item(string key)
{
    next = pre = NULL;
    value = key;
}

class HashTable
{
private:

    // Array is a reference to an array of Linked Lists.
    vector<Item*> array[512];

    // Length is the size of the Hash Table array.
    int length;

    // Returns an array location for a given item key.
    int hash( string &itemKey );

public:

    // Constructs the empty Hash Table object.
    // Array length is set to 13 by default.
    HashTable( int tableLength = 512 );

    // Adds an item to the Hash Table.
    bool insertItem(string &newItem, Item **start );
    bool insertItem_FIFO(string &newItem, Item **start);

    // Deletes an Item by key from the Hash Table.
    // Returns true if the operation is successful.
    void removeItem(Item *removeItem );

    // Returns the number of locations in the Hash Table.
    int getLength();

    // Returns the number of Items in the Hash Table.
    int getNumberOfItems();

    void clearItems();
    void changeLength(int newlength);

    // De-allocates all memory used for the Hash Table.
    ~HashTable();
};
HashTable::HashTable( int tableLength )
{
    length = tableLength;
}

// Returns an array location for a given item key.
int HashTable::hash( string &itemKey )
{
    int value = 0;
    for ( int i = 0; i < itemKey.length(); i++ )
        value += itemKey[i];
    return (value * itemKey.length() ) % length;
}

// Adds an item to the Hash Table.
bool HashTable::insertItem_FIFO(string &newItem, Item **start )
{
    int index = hash(newItem);
    for(vector<Item*>::iterator i = array[index].begin(); i != array[index].end(); i++)
    {
        if((*i)->value == newItem)
            return false;
    }
    Item *tmp = new Item(newItem);
    tmp->next = *start;
    *start = tmp->next->pre = tmp;
    array[ index ].push_back(tmp);
    return true;
}

bool HashTable::insertItem(string &newItem, Item **start )
{
    int index = hash(newItem);
    for(vector<Item*>::iterator i = array[index].begin(); i != array[index].end(); i++)
    {
        if((*i)->value == newItem)
        {
            if((*i) != *start)
            {
                (*i)->next->pre = (*i)->pre;
                (*i)->pre->next = (*i)->next;
                (*i)->next = *start;
                (*i)->pre = NULL;
                *start = (*i)->next->pre = (*i);
            }
            return false;
        }
    }
    Item *tmp = new Item(newItem);
    tmp->next = *start;
    *start = tmp->next->pre = tmp;
    array[ index ].push_back(tmp);
    return true;
}

// Deletes an Item by key from the Hash Table.
// Returns true if the operation is successful.
void HashTable::removeItem(Item *removeItem )
{
    int index = hash(removeItem->value);
    for(vector<Item*>::iterator i = array[index].begin(); i != array[index].end(); i++)
    {
        if(*i == removeItem) {
            delete *i;
            array[index].erase(i);
            break;
        }
    }
}

// Returns the number of locations in the Hash Table.
int HashTable::getLength()
{
    return length;
}

// Returns the number of Items in the Hash Table.
int HashTable::getNumberOfItems()
{
    int itemCount = 0;
    for ( int i = 0; i < length; i++ )
    {
        itemCount += array[i].size();
    }
    return itemCount;
}

void HashTable::clearItems()
{
    for(int i = 0; i < length; i++)
    {
        for(vector<Item*>::iterator j = array[i].begin(); j != array[i].end(); j++)
            delete *j;
        array[i].clear();
    }
}

void HashTable::changeLength(int newlength)
{
    if(newlength <= 512)
        length = newlength;
}

// De-allocates all memory used for the Hash Table.
HashTable::~HashTable()
{
}


int main() {
    fstream trace;
    char op;
    string addr;
    HashTable hshs;
    int miss, hit, full;
    Item *start;
    Item end(" ");

    miss = hit = full = 0;
    trace.open("trace.txt", fstream::in);

    cout << "FIFO---" << endl;
    cout << "size          miss          hit           page fault ratio" << endl;

    for(int size = 64; size <= 512; size*=2)
    {
        start = &end;
        while(trace >> op >> addr)
        {
            addr.resize(5);
            //find whether addr is in the page

            if(hshs.insertItem_FIFO(addr, &start))
            {
                full++;
                miss++;

                if(full > size)
                {
                    Item *tmp = end.pre;
                    tmp->pre->next = &end;
                    end.pre = tmp->pre;
                    hshs.removeItem(tmp);
                    full--;
                }
            }
            else
                hit++;
        }

        string tmp = to_string(size);
        for(int i = tmp.size(); i < 14; i++)
            tmp+=" ";
        tmp+=to_string(miss);
        for(int i = to_string(miss).size(); i < 14; i++)
            tmp+=" ";
        tmp+=to_string(hit);
        for(int i = to_string(hit).size(); i < 14; i++)
            tmp+=" ";
        cout << tmp << (double)miss/(double)(miss+hit) << endl;

        trace.clear();
        trace.seekg(trace.beg);
        hshs.clearItems();
        miss = hit = full = 0;
    }

    cout << "LRU---" << endl;
    cout << "size          miss          hit           page fault ratio" << endl;
    for(int size = 64; size <= 512; size*=2)
    {
        start = &end;

        while(trace >> op >> addr)
        {
            addr.resize(5);

            if(hshs.insertItem(addr, &start))
            {
                full++;
                miss++;

                if(full > size)
                {
                    Item *tmp = end.pre;
                    tmp->pre->next = &end;
                    end.pre = tmp->pre;
                    hshs.removeItem(tmp);
                    full--;
                }
            }
            else
                hit++;
        }

        string tmp = to_string(size);
        for(int i = tmp.size(); i < 14; i++)
            tmp+=" ";
        tmp+=to_string(miss);
        for(int i = to_string(miss).size(); i < 14; i++)
            tmp+=" ";
        tmp+=to_string(hit);
        for(int i = to_string(hit).size(); i < 14; i++)
            tmp+=" ";
        cout << tmp << (double)miss/(double)(miss+hit) << endl;

        trace.clear();
        trace.seekg(trace.beg);
        hshs.clearItems();
        miss = hit = full = 0;
    }
    trace.close();
    return 0;
}