//
// Created by Weiwei Jiang on 18/06/03.
//

#include "NIRScanner.h"

int main() {
    NIRScanner scanner;
    scanner.readVersion();
    scanner.scan(true);
}

