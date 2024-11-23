//
//  main.m
//  SadakKaLadaakoo-Mac
//
//  Created by Sarthak Kamboj on 11/18/24.
//

#import <Foundation/Foundation.h>

#include "platform/macos/skl_macos.h"
#include "platform/app.h"

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        mac_init_ctx_t init_ctx{};
        app_run(init_ctx);
    }
    return 0;
}
