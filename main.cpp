// #include <bits/stdc++.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <ostream>
#include <sstream> 

using namespace std;

class Node
{
public:
    string key;
    int val;
    Node *next, *prev;
    Node(string k, int v)
    {
        key = k;
        val = v;
        next = nullptr;
        prev = nullptr;
    }
};

class CustomHashMap
{
public:
    const static int p = 29;
    const static int MAXHASH = 30; // 30:31, 300:293, 1000:1009
    const static int MAXWORDLEN = 48;
    long long p_power[MAXWORDLEN]; // precomputed powers of p
    Node *bucket[MAXHASH] = {};

    CustomHashMap()
    {
        p_power[0] = 1;
        for (int i = 1; i < MAXWORDLEN; i++)
        {
            p_power[i] = (p_power[i - 1] * p) % MAXHASH;
        }
    }

    long long hash(const string &key)
    {
        long long hash_val = 0;
        int key_len = key.length();
        for (int i = 0; i < key_len; i++)
        {
            hash_val = (hash_val + (key[i] - 'a' + 1) * p_power[i]) % MAXHASH;
        }
        return hash_val;
    }

    void insert(string key, int val)
    {
        long long hash_val = hash(key);
        Node *node = new Node(key, val);
        if (bucket[hash_val])
        {
            Node *headNode = bucket[hash_val];
            node->next = headNode;
            headNode->prev = node;
            headNode = node;
            bucket[hash_val] = headNode;
        }
        else
        {
            bucket[hash_val] = node;
        }
    }

    int find(string key)
    {
        int out = -1;
        try
        {
            out = findNodeWithKey(key).val;
        }
        catch (const out_of_range& e) {
            // cerr << "Error: " << e.what() << '\n';
        }
        return out;
    }

    void del(string key)
    {
        long long hash_val = hash(key);
        if (bucket[hash_val])
        {
            Node *currentNode = bucket[hash_val];
            Node *headNode = currentNode;
            while (currentNode != nullptr && currentNode->key != key)
            {
                currentNode = currentNode->next;
            }

            if (currentNode == nullptr)
            {
                return;
            }

            if (currentNode->prev != nullptr)
            {
                currentNode->prev->next = currentNode->next;
            }
            else
            {
                headNode = currentNode->next;
                bucket[hash_val] = headNode;
            }

            if (currentNode->next != nullptr)
            {
                currentNode->next->prev = currentNode->prev;
            }
            delete currentNode;
        }
    }

    void increase(string key)
    {
        Node &currentNode = findNodeWithKey(key);
        currentNode.val++;
    }

    vector<string> listAllKeys()
    {
        vector<string> keyList = {};

        for (int i = 0; i < MAXHASH; i++)
        {
            if (bucket[i])
            {
                if(bucket[i] != nullptr) {
                    Node *currentNode = bucket[i];
                    while (currentNode != nullptr)
                    {
                        keyList.push_back(currentNode->key);
                        currentNode = currentNode->next;
                    }
                }
                else
                {
                    keyList.push_back(bucket[i]->key);
                }
            }
        }
        return keyList; 
    }

    int histogram()
    {   
        int total = 0;
        int max_count = 0;
        ofstream histogramfile("histogram.txt");
        if(histogramfile.is_open())
        {
            for (int i = 0; i < MAXHASH; i++)
            {
                if (bucket[i])
                {   
                    int count = 0;
                    if(bucket[i] != nullptr) {
                        Node *currentNode = bucket[i];
                        while (currentNode != nullptr)
                        {
                            ++count;
                            currentNode = currentNode->next;
                        }
                        histogramfile << "pos: " << i << " length: " << count << endl;
                    }
                    else
                    {
                        histogramfile << "pos: " << i << " length: " << ++count << endl;
                    }
                    max_count = count > max_count ? count : max_count;
                    total = total + count;
                }
                else
                {
                    histogramfile << "pos: " << i << " length: " << 0 << endl;
                }
            }
            histogramfile << "mean length of linked lists: " << total / MAXHASH << endl;
            histogramfile.close();
        }
        else
        {
            cout << "Unable to create histogram.txt file." << endl;
        }
        return max_count;
    }

private:
    Node &findNodeWithKey(string key)
    {
        long long hash_val = hash(key);
        if (bucket[hash_val])
        {
            Node *currentNode = bucket[hash_val];
            do
            {
                if (currentNode->key == key)
                {
                    return *currentNode;
                }
                else if (currentNode->next)
                {
                    currentNode = currentNode->next;
                }
                else
                {
                    break;
                }
            } while (true);
        }
        throw out_of_range("Key not found");
    }

};

vector<string> toTokenVec(string text)
{
    istringstream iss(text);
    vector<string> tokens;
    string token;

    while (iss >> token)
    {
        string cleanedToken = "";
        for (char &c : token)
        {
            if (c >= 65 && c <= 90)
            {
                cleanedToken += (c + 32);
            }
            else if ((c >= 97 && c <= 122)) // || (c >= 48 && c <= 57)
            {
                cleanedToken += c;
            }
        }
        if (!cleanedToken.empty())
        {
            tokens.push_back(cleanedToken);
        }
    }
    return tokens;
}

string readTextFile(string fileName)
{
    ifstream infile(fileName);
    stringstream buffer;
    buffer << infile.rdbuf();
    return buffer.str();
}

int main(int argc, char *argv[])
{
    string file_contents = readTextFile("../../text_file.txt"); // argv[1]
    vector<string> tokens = toTokenVec(file_contents);
    CustomHashMap customHashMap;
    int i = 0;
    for (const auto &token : tokens)
    {
        int val;
        val = customHashMap.find(token);
        if (val != -1) 
        {
            customHashMap.increase(token);
        }
        else
        {
            customHashMap.insert(token, 1);
        }
        
    }
    vector<string> allKeys = customHashMap.listAllKeys();

    ofstream hashMapKeyValueFile("hashMapKeyValues.txt");
    for (const auto &key : allKeys)
    {
        cout << key << " : " << customHashMap.find(key) << endl;
        hashMapKeyValueFile << key << " : " << customHashMap.find(key) << endl;
    }
    hashMapKeyValueFile.close();
    cout << "-------------------------------------------------------------------------------------" << endl;
    int max_val = customHashMap.histogram();
    cout << "length of longest linked list: " << max_val << endl;
    return 0;
}
