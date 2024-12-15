//
//  skl_macos.mm
//  SadakKaLadaakoo-Mac
//
//  Created by Sarthak Kamboj on 11/18/24.
//

#include "skl_macos.h"

#include "defines.h"
#include "geometry/model.h"
#include "shaders/macos/shader.h"
#include "entities.h"
#include "skl_math.h"

extern app_state_t g_app_state;

@implementation GameWindow

@synthesize mtk_view;

-(instancetype)init:(NSRect)content_rect mtk_view:(MTKView *)_mtk_view {
    self = [super initWithContentRect:content_rect styleMask:NSWindowStyleMaskTitled|NSWindowStyleMaskClosable backing:NSBackingStoreBuffered defer:false];
    if (self) {
        [self setAcceptsMouseMovedEvents:true];
        mtk_view = _mtk_view;
    }
    return self;
}

- (void)mouseDown:(NSEvent *)event {
    SKL_LOG("mouse down detected");
}

- (void)mouseMoved:(NSEvent*)event {
    NSEventType event_type = [event type];
    if (event_type != NSEventTypeMouseMoved) {
        SKL_LOG("event type not mouse moved");
    } else {
        NSPoint point = [event locationInWindow];
        NSRect bounds = [mtk_view bounds];
        // SKL_LOG("mouse pos: %f %f", point.x, point.y);
        // SKL_LOG("bounds: %f %f", bounds.size.width, bounds.size.height);
        g_app_state.input.mouse_x = point.x / bounds.size.width;
        g_app_state.input.mouse_y = 1.0f - (point.y / bounds.size.height);
    }
}

@end

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
    
    NSRect content_rect;
    content_rect.origin.x = 0;
    content_rect.origin.y = 0;
    content_rect.size.width = width;
    content_rect.size.height = height;
    
    mtk_view = [[MTKView alloc] initWithFrame:content_rect device:metal_device];
    [mtk_view setColorPixelFormat:MTLPixelFormatBGRA8Unorm];
    
    MTLClearColor clear_color = {0,0.0f,0.0f,1};
    [mtk_view setClearColor:clear_color];
    
    custom_mtk_delegate = [[CustomMTKDelegate alloc] initWithDevice:metal_device];
    custom_mtk_delegate.metal_device = metal_device;
    [mtk_view setDelegate:custom_mtk_delegate];
    
    NSString* title_ns = [NSString stringWithUTF8String:title];
    window = [[GameWindow alloc] init:content_rect mtk_view:mtk_view];
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
@synthesize vert_buffer;
@synthesize uniform_buffer;
@synthesize render_pipeline_state;

- (instancetype)initWithDevice:(id<MTLDevice>) _metal_device {
    self = [super init];
    if (self) {
        metal_device = _metal_device;
        metal_cmd_queue = [metal_device newCommandQueue];
        
#if false
        skl_vert_t verts[3] = {
            { { 0.0f, 0.25f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
            { { 0.25f, -0.25f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
            { { -0.25f, -0.25f,  0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
        };
#endif
        
        NSBundle* asset_bundle = [NSBundle mainBundle];
        NSString* box_path_ns = [asset_bundle pathForResource:@"box" ofType:@"glb"];
        const char* box_path = [box_path_ns UTF8String];
        
        model_t model = load_model(box_path);
        
        vert_buffer = [metal_device newBufferWithLength:model.num_verts*sizeof(skl_vert_t) options:MTLResourceStorageModeManaged];
        void* vert_buffer_ptr = [vert_buffer contents];
        memcpy(vert_buffer_ptr, model.verts, model.num_verts*sizeof(skl_vert_t));
        [vert_buffer didModifyRange:NSMakeRange(0, [vert_buffer length] )];
        
        uniform_buffer = [metal_device newBufferWithLength:sizeof(uniform_data_t) options:MTLResourceStorageModeManaged];
        void* uniform_buffer_ptr = [uniform_buffer contents];
        memset(uniform_buffer_ptr, 0, sizeof(uniform_data_t));
        [uniform_buffer didModifyRange:NSMakeRange(0, [uniform_buffer length] )];
        
        id<MTLLibrary> metal_libray = [metal_device newDefaultLibrary];
        NSString* vert_name = @"vertex_main";
        NSString* frag_name = @"fragment_main";
        id<MTLFunction> vert_func = [metal_libray newFunctionWithName:vert_name];
        id<MTLFunction> frag_func = [metal_libray newFunctionWithName:frag_name];
        
        MTLRenderPipelineDescriptor * pipeline_desc = [[MTLRenderPipelineDescriptor alloc] init];
        [pipeline_desc setVertexFunction:vert_func];
        [pipeline_desc setFragmentFunction:frag_func];
        MTLRenderPipelineColorAttachmentDescriptorArray* color_atts = [pipeline_desc colorAttachments];
        MTLRenderPipelineColorAttachmentDescriptor* color_att_desc = [color_atts objectAtIndexedSubscript:0];
        [color_att_desc setPixelFormat:MTLPixelFormatBGRA8Unorm];
        
        render_pipeline_state = [metal_device newRenderPipelineStateWithDescriptor:pipeline_desc error:nil];

    }
    return self;
}

- (void) drawInMTKView:(MTKView *) view {
    
    update();
    
    id<MTLCommandBuffer> cmd_buffer = [metal_cmd_queue commandBuffer];
    id<MTLRenderCommandEncoder> render_cmd_encoder = [cmd_buffer renderCommandEncoderWithDescriptor:view.currentRenderPassDescriptor];
    [render_cmd_encoder setRenderPipelineState:render_pipeline_state];
    [render_cmd_encoder setTriangleFillMode:MTLTriangleFillModeLines];
    
    for (int i = 0; i < 1; i++) {
        transform_t* transform = get_transform_from_id(i);
        render_options_t* render_options = get_render_options(i);
        
        void* uniform_buffer_ptr = [uniform_buffer contents];
        uniform_data_t uniform_data{};
        uniform_data.mouse_pos_x = (transform->screen_x_pos * 2.f) - 1.f;
        uniform_data.mouse_pos_y = (transform->screen_y_pos * -2.f) + 1.f;
        
        uniform_data.color[0] = render_options->color[0];
        uniform_data.color[1] = render_options->color[1];
        uniform_data.color[2] = render_options->color[2];
        
        float near_plane = 0.01f;
        float far_plane = 30.0f;
        
        matrix_t scale = transform_mat( 5.f, 1.f );
        matrix_t pers = pers_mat( near_plane, far_plane );
        matrix_t ndc = ndc_mat( near_plane, far_plane, 60.0f, g_app_state.window_info.width / g_app_state.window_info.width );
        
        uniform_data.world_mat = matrix_to_simd_mat( scale );
        uniform_data.pers_mat = matrix_to_simd_mat( pers );
        uniform_data.ndc_mat = matrix_to_simd_mat( ndc );
        
        memcpy(uniform_buffer_ptr, &uniform_data, sizeof(uniform_data_t));
        [uniform_buffer didModifyRange:NSMakeRange(0, [uniform_buffer length] )];
        
        [render_cmd_encoder setVertexBuffer:vert_buffer offset:0 atIndex:0];
        [render_cmd_encoder setVertexBuffer:uniform_buffer offset:0 atIndex:1];
        
        [render_cmd_encoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:36];
    }
    [render_cmd_encoder endEncoding];
        
    [cmd_buffer presentDrawable:view.currentDrawable];
    [cmd_buffer commit];
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size { 
    SKL_LOG("size change")
}

@end

void app_run_platform_specific(const window_info_t& window_info, const mac_init_ctx_t& init_ctx) {
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
