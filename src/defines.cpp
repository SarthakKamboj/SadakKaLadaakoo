//
//  defines.cpp
//  SadakKaLadaakoo-Mac
//
//  Created by Sarthak Kamboj on 12/12/24.
//

#include "defines.h"

NSString* c_string_to_ns_string(const char* c_str) {
    return [NSString stringWithUTF8String:c_str];
}

const char* ns_string_to_c_string(NSString* ns_str) {
    return [ns_str UTF8String];
}
