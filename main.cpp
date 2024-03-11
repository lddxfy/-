#include <iostream>
#include "skiplist.h"
#include <string>
using namespace std;

int main(){
    SkipList<int,string> skl(5);
    //skl.load_file();
    skl.insert_element(1,"hello");
    skl.insert_element(65,"hy");
    skl.insert_element(21,"cjq");
    skl.insert_element(18,"world!");
    skl.insert_element(56,"test");
    //skl.dump_file();
    skl.display_list();
    //skl.load_file();
    skl.search_element(21);
    skl.search_element(33);
    skl.delete_node(1);
    skl.display_list();
    //skl.delete_node(65);
    //skl.display_list();
    return 0;
}