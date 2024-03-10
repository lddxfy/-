#include <iostream>
#include "skiplist.h"
#include <string>
using namespace std;

int main(){
    SkipList<int,string> skl(5);
    //skl.load_file();
    //skl.insert_element(1,"hello");
    //skl.insert_element(65,"hy");
    //skl.insert_element(21,"cjq");
    //skl.dump_file();
    skl.load_file();
    skl.display_list();
    return 0;
}