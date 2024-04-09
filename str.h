#ifndef _str_H_
#define _str_H_
#include <vector>

class str
{
    char *data;
    int length;
    int alphabet_loc(char);
    int strlen(const char *);
    // preprocess:
    int match_length(int, int);
    std::vector<int> fundamental_preprocess();
    std::vector<std::vector<int>> bad_char_table();
    std::vector<int> good_suffix_table();
    std::vector<int> full_shift_table();
    
public:
    const char *get_data() const;
    str(const char *);
    str();
    ~str();
    char &operator[](int);
    str &operator=(str);
    str& operator+=(char);
    friend std::ostream &operator<<(std::ostream &, const str &);
    friend std::istream &operator>>(std::istream &, str &);
    str reversed();
    int get_length();
    void strcat(str);
    std::vector<int> search_BM(str P);
    void read(const char *);
    void getline(std::istream &,char);
};

std::vector<int> reverse_vector(std::vector<int>, int);

#endif