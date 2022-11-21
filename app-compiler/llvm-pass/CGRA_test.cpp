#include "cgra.h"
#include <string>
#include <iostream>

int main(){
    CGRA one("config.bit", "mapped_dfgio.txt", "Parameter.txt");
    one.Printports();
    one.Printcin();
    return 0;
}