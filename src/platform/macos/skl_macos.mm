//
//  skl_macos.mm
//  SadakKaLadaakoo-Mac
//
//  Created by Sarthak Kamboj on 11/18/24.
//

#include "skl_macos.h"

#include "defines.h"

#include <AppKit/NSWindow.h>

@implementation AppDelegate

@synthesize width;
@synthesize height;
@synthesize metal_device;
@synthesize mtk_view;
@synthesize window;
@synthesize title;
@synthesize custom_mtk_delegate;

-(void) applicationDidFinishLaunching: (NSNotification*)notification {
    NSApplication* app = [notification object];
    SKL_LOG("app in notif is %p", app);
    
    NSRect content_rect;
    content_rect.origin.x = 0;
    content_rect.origin.y = 0;
    content_rect.size.width = width;
    content_rect.size.height = height;
    
    window = [[NSWindow alloc] initWithContentRect:content_rect styleMask:NSWindowStyleMaskTitled|NSWindowStyleMaskClosable backing:NSBackingStoreBuffered defer:false];
    
    mtk_view = [[MTKView alloc] initWithFrame:content_rect device:metal_device];
    [mtk_view setColorPixelFormat:MTLPixelFormatBGRA8Unorm];
    
    MTLClearColor clear_color = {0,0.5f,1.0f,1};
    [mtk_view setClearColor:clear_color];
    
    custom_mtk_delegate = [[CustomMTKDelegate alloc] initWithDevice:metal_device];
    custom_mtk_delegate.metal_device = metal_device;
    [mtk_view setDelegate:custom_mtk_delegate];
    
    NSString* title_ns = [NSString stringWithUTF8String:title];
    [window setTitle:title_ns];
    [window setContentView:mtk_view];
    [window makeKeyAndOrderFront:nil];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
    return true;
}

@end

@implementation CustomMTKDelegate

@synthesize metal_device;
@synthesize metal_cmd_queue;

- (instancetype)initWithDevice:(id<MTLDevice>) _metal_device {
    self = [super init];
    if (self) {
        metal_device = _metal_device;
        metal_cmd_queue = [metal_device newCommandQueue];
    }
    return self;
}

- (void) drawInMTKView:(MTKView *) view {
    SKL_LOG("drawing");
    id<MTLCommandBuffer> cmd_buffer = [metal_cmd_queue commandBuffer];
    id<MTLRenderCommandEncoder> render_cmd_encoder = [cmd_buffer renderCommandEncoderWithDescriptor:view.currentRenderPassDescriptor];
    [render_cmd_encoder endEncoding];
    [cmd_buffer presentDrawable:view.currentDrawable];
    [cmd_buffer commit];
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size { 
    SKL_LOG("size change")
}

@end

void init_platform_specific(const window_info_t& window_info, const mac_init_ctx_t& init_ctx) {
    AppDelegate* app_delegate = [[AppDelegate alloc] init];
    app_delegate.width = window_info.width;
    app_delegate.height = window_info.height;
    app_delegate.title = window_info.name;
    id<MTLDevice> created_metal_device = MTLCreateSystemDefaultDevice();
    if (!created_metal_device) {
        SKL_LOG("could not create metal device");
        return;
    }
    app_delegate.metal_device = created_metal_device;
    NSApplication* app = [NSApplication sharedApplication];
    [app setDelegate:app_delegate];
    [NSApp run];
    
}

void poll_events() {
    
}

void render_frame() {
    
}
