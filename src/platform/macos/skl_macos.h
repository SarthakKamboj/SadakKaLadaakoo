//
//  skl_macos.h
//  SadakKaLadaakoo-Mac
//
//  Created by Sarthak Kamboj on 11/18/24.
//

#include "../app.h"

#import <AppKit/NSApplication.h>
#import <Metal/Metal.h>
#import <MetalKit/MTKView.h>

struct mac_init_ctx_t {};

@class CustomMTKDelegate;

@interface AppDelegate : NSObject<NSApplicationDelegate>

@property(assign) int width;
@property(assign) int height;
@property(assign) const char* _Nonnull title;
@property(retain) id<MTLDevice> _Nonnull metal_device;
@property NSWindow* _Nonnull window;
@property MTKView* _Nonnull mtk_view;
@property CustomMTKDelegate* _Nonnull custom_mtk_delegate;

-(void)applicationDidFinishLaunching:(NSNotification*)notification;
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender;

@end

@interface CustomMTKDelegate : NSObject<MTKViewDelegate>

@property(retain) id<MTLDevice> _Nonnull metal_device;
@property id<MTLCommandQueue> _Nonnull metal_cmd_queue;
@property id<MTLBuffer> _Nonnull vert_buffer;
@property id<MTLRenderPipelineState> _Nonnull render_pipeline_state;

- (instancetype)initWithDevice:(id<MTLDevice>) metal_device;
- (void)drawInMTKView:(MTKView *) view;
- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size;

@end

void app_run_platform_specific(const window_info_t& window_info, const mac_init_ctx_t& init_ctx);
