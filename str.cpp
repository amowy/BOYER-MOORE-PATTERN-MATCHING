#include "str.h"
#include <iostream>
#include <fstream>

using namespace std;

int ALPHABET_SIZE = 257;

int str::alphabet_loc(char a)
{
    if(a < 0)
        return (int)a + ALPHABET_SIZE;
    return (int) a;
}

int str::strlen(const char *txt)
{
    int length = 0;
    while (txt[length] != '\0')
        length++;
    return length;
}

/*---------PREPROCESS---------*/

int str::match_length(int index_1, int index_2)
{
    if (index_1 == index_2)
    {
        return length - index_1;
    }
    int match_count = 0;
    while (index_1 < length and index_2 < length and data[index_1] == data[index_2])
    {
        match_count++;
        index_1++;
        index_2++;
    }
    return match_count;
}

vector<int> str::fundamental_preprocess()
{
    vector<int> z(length, 0);
    if (length == 0) // empty
    {
        return z;
    }
    if (length == 1) // single letter
    {
        z[0] = 1;
        return z;
    }
    z[0] = length;
    z[1] = match_length(0, 1);
    for (int i = 2; i <= z[1]; i++) // valami
    {
        z[i] = z[1] - i + 1;
    }
    int l = 0, r = 0;
    int k, a, b;
    for (int i = 2 + z[1]; i < z[0]; i++)
    {
        if (i <= r)
        {
            k = i - l;
            b = z[k];
            a = r - i + 1;
            if (b < a)
            {
                z[i] = b;
            }
            else
            {
                z[i] = a + match_length(a, r + 1);
                l = i;
                r = i + z[i] - 1;
            }
        }
        else
        {
            z[i] = match_length(0, i);
            if (z[i] > 0)
            {
                l = i;
                r = i + z[i] - 1;
            }
        }
    }
    return z;
}

vector<vector<int>> str::bad_char_table()
{
    vector<vector<int>> R(ALPHABET_SIZE, vector<int>(0));
    if (length == 0)
    {
        return R;
    }
    fill(R.begin(), R.end(), vector<int>(1, -1));
    vector<int> alpha(ALPHABET_SIZE, -1);
    for (int i = 0; i < length; i++)
    {
        alpha[alphabet_loc(data[i])] = i;
        for (int j = 0; j < alpha.size(); j++)
        {
            R[j].push_back(alpha[j]);
        }
    }
    return R;
}

vector<int> str::good_suffix_table()
{
    vector<int> L(length, -1);
    vector<int> N = reversed().fundamental_preprocess();
    N = reverse_vector(N, length);
    int i;
    for (int j = 0; j < length - 1; j++)
    {
        i = length - N[j];
        if (i != length)
        {
            L[i] = j;
        }
    }
    return L;
}

vector<int> str::full_shift_table()
{
    vector<int> F(length, 0);
    vector<int> Z = fundamental_preprocess();
    int longest = 0;
    for (int i = 0; i < length; i++)
    {
        longest = (Z[length - 1 - i] == i + 1) ? max(Z[length - 1 - i], longest) : longest;
        F[length - i - 1] = longest;
    }
    return F;
}

/*---------------------------*/

const char *str::get_data() const
{
    return data;
}

str::str(const char *txt)
{
    length = strlen(txt);
    data = new char[length + 1];
    for (int i = 0; i < length; i++)
    {
        data[i] = txt[i];
    }
    data[length] = '\0';
}

str::str()
{
    data = new char[1];
    data[0] = '\0';
    length = 0;
}

str::~str()
{
    delete[] data;
}

char &str::operator[](int index)
{
    if (index < 0 or index >= length)
    {
        cerr << "out of index";
        exit(1);
    }
    return data[index];
}

str &str::operator=(str other)
{
    if (this == &other)
    {
        return *this;
    }
    delete[] data;
    length = other.length;
    data = new char[length + 1];
    for (int i = 0; i < length; i++)
        data[i] = other.data[i];
    return *this;
}

ostream &operator<<(ostream &os, const str &s)
{
    os << s.get_data();
    return os;
}

istream &operator>>(istream &is, str &s)
{
    char buffer[1024];
    is >> buffer;
    s = buffer;
    return is;
}

str str::reversed()
{
    str s;
    s.length = length;
    s.data = new char[length + 1];
    int i = 0;
    while (data[i] != '\0')
    {
        s.data[i] = data[length - i - 1];
        i++;
    }
    s.data[i] = '\0';
    return s;
}

int str::get_length()
{
    return length;
}

void str::strcat(str other)
{
    char* temp = new char[length + other.get_length() + 1];
    for(int i = 0; i < length; i++)
        temp[i] = data[i];
    for(int i = 0; i <= other.get_length();i++)
        temp[i + length] = other.data[i];
    delete[] data;
    data = temp;
    length += other.get_length();

}

vector<int> str::search_BM(str P)
{
    vector<int> matches(0);
    if (P.length == 0 or length == 0 or length < P.length)
    {
        return matches;
    }

    // Preprocessing:
    vector<vector<int>> R = P.bad_char_table();
    vector<int> L = P.good_suffix_table();
    vector<int> F = P.full_shift_table();

    int k = P.length - 1, previous_k = -1;
    int i, h, char_shift = 0, suffix_shift = 0, shift;
    while (k < length)
    {
        i = P.length - 1;
        h = k;
        while (i >= 0 and h > previous_k and P[i] == data[h])
        {
            i--;
            h--;
        }
        if (i == -1 or h == previous_k) // match found
        {
            matches.push_back(k - P.length + 1);
            k += (P.length > 1) ? (P.length - F[1]) : 1;
        }
        else // No match, shift by max of bad character and good suffix rules
        {
            char_shift = i - R[alphabet_loc(data[h])][i];
            if (i + 1 == P.length) // mismatch
            {
                suffix_shift = 1;
            }
            else if (L[i + 1] == -1) // Matched suffix does not appear anywhere in P
            {
                suffix_shift = P.length - F[i + 1];
            }
            else // matched suffix appears in P
            {
                suffix_shift = P.length - 1 - L[i + 1];
            }
            shift = max(char_shift, suffix_shift);
            previous_k = (shift >= i + 1) ? k : previous_k;
            k += shift;
        }
    }
    return matches;
}

void str::read(const char *FileName)
{
    ifstream input(FileName);
    if (input.is_open())
    {
        input.seekg(0, ios::end);
        int size = input.tellg();
        input.seekg(0, ios::beg);
        data = new char[size];
        length = size - 1;
        input.read(data, size);
        return;
    }
    cerr << "Can't open file \n";
    exit(1);
}

void str::getline(std::istream& is,char limit = '\n')
{
    char c;
    while(is.get(c) && c != limit)
    {
        operator+=(c);
    }
}

str& str::operator+=(char c)
{
    length++;
    char* newData = new char[length + 1];
    int i = 0;
    while(data[i] != '\0')
    {
        newData[i] = data[i];
        i++;
    }
    newData[i] = c;
    newData[i + 1] = '\0';
    delete[] data;
    data = newData;
    return *this;
}

/*--------VEKTOR--------*/
vector<int> reverse_vector(vector<int> vec, int size)
{
    vector<int> seged(size);
    for (int i = 0; i < size; i++)
    {
        seged[i] = vec[size - 1 - i];
    }
    return seged;
}
