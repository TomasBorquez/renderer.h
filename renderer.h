/* MIT License

  renderer.h - A single-header library for using Clay with Raylib
  Version - 2025-05-11 (0.1.1):
  https://github.com/TomasBorquez/renderer.h

  Usage:
    #define CLAY_IMPLEMENTATION
    #include "clay.h"
    #define RENDERER_IMPLEMENTATION
    #include "renderer.h"
*/

#pragma once

#include "clay.h"
#include "raylib.h"
#include "raymath.h"
#include "stdio.h"
#include "stdlib.h"
#include <assert.h>
#include <math.h>
#include <string.h>

#ifdef _WIN32
#define NOGDI
#define NOUSER
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

/*
  Default raylib_renderer.c stuff
  Source: https://github.com/nicbarker/clay/blob/main/renderers/raylib/clay_renderer_raylib.c
*/
#define CLAY_RECTANGLE_TO_RAYLIB_RECTANGLE(rectangle)                                                                                                                                                                                          \
  (Rectangle) {                                                                                                                                                                                                                                \
    .x = rectangle.x, .y = rectangle.y, .width = rectangle.width, .height = rectangle.height                                                                                                                                                   \
  }

#define CLAY_COLOR_TO_RAYLIB_COLOR(color)                                                                                                                                                                                                      \
  (Color) {                                                                                                                                                                                                                                    \
    .r = (uint8_t)roundf(color.r), .g = (uint8_t)roundf(color.g), .b = (uint8_t)roundf(color.b), .a = (uint8_t)roundf(color.a)                                                                                                                 \
  }

#define RAYLIB_VECTOR2_TO_CLAY_VECTOR2(vector)                                                                                                                                                                                                 \
  (Clay_Vector2) {                                                                                                                                                                                                                             \
    .x = vector.x, .y = vector.y,                                                                                                                                                                                                              \
  }

Camera Raylib_camera;

typedef enum { CUSTOM_LAYOUT_ELEMENT_TYPE_3D_MODEL } CustomLayoutElementType;

typedef struct {
  Model model;
  float scale;
  Vector3 position;
  Matrix rotation;
} CustomLayoutElement_3DModel;

typedef struct {
  CustomLayoutElementType type;
  union {
    CustomLayoutElement_3DModel model;
  } customData;
} CustomLayoutElement;

Ray GetScreenToWorldPointWithZDistance(Vector2 position, Camera camera, int screenWidth, int screenHeight, float zDistance);
static inline Clay_Dimensions Raylib_MeasureText(Clay_StringSlice text, Clay_TextElementConfig *config, void *userData);
void Clay_Raylib_Initialize(int width, int height, const char *title, unsigned int flags);
void Clay_Raylib_Render(Clay_RenderCommandArray renderCommands, Font *fonts);
Clay_String toClayString(char *str);

/* Arena Implementation, inspired from:
   https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/
*/
typedef struct {
  int8_t *buffer;
  size_t bufferLength;
  size_t prevOffset;
  size_t currOffset;
} Arena;

Arena ArenaInit(size_t size);
void *ArenaAlloc(Arena *arena, size_t size);
void ArenaFree(Arena *arena);
void ArenaReset(Arena *arena);

// This makes sure right alignment on 86/64 bits
#define DEFAULT_ALIGNMENT (2 * sizeof(void *))

/* Our renderer.h specifics */
typedef struct {
  int32_t totalMemorySize;
  Clay_Arena clayMemory;
  Font fonts[4];
  bool reinitialize;
  bool debugEnabled;
  bool shouldClose;
} Renderer;
extern Renderer renderer;

void HandleClayErrors(Clay_ErrorData errorData);
static void initDraw();

void ScrollContainerByY(char *containerName, float deltaY);
void ScrollContainerTop(char *containerName);
void ScrollContainerBottom(char *containerName);
void ScrollContainerByX(char *containerName, float deltaX);

Clay_String s(const char *msg);

typedef struct {
  int32_t height;
  int32_t width;
  char *windowName;
  char *fontPath;
} RenderOptions;

typedef void (*Callback)(void);

enum {
  FONT_18,
  FONT_20,
  FONT_22,
  FONT_24,
};

void RenderSetup(RenderOptions options, Callback updateCallback, Callback drawCallback);

#ifdef __clang__
/*  Printf like warnings on format */
#define FORMAT_CHECK(fmt_pos, args_pos) __attribute__((format(printf, fmt_pos, args_pos)))
#else
#define FORMAT_CHECK(fmt_pos, args_pos)
#endif

Clay_String F(Arena *arena, const char *format, ...) FORMAT_CHECK(2, 3);

typedef struct {
  Clay_Color color;
  char width[6];
} Border;

typedef struct {
  char *id;

  // Misc
  Clay_Color bg;
  uint16_t gap;
  bool reverse; // ! Not yet implemented working
  char scroll[1];

  // Padding
  uint16_t p;
  uint16_t pb;
  uint16_t pt;
  uint16_t pl;
  uint16_t pr;
  uint16_t py;
  uint16_t px;

  // Align
  char align[2];

  // Border radius
  char borderRadius[6]; // ! Not yet working for individual sides
  Border border;

  // Sizing
  char *w;
  char *h;
} ComponentOptions;

static Clay_ElementDeclaration ParseComponentOptions(ComponentOptions options, Clay_ElementDeclaration defaultOptions);

// Better MACROS
#define TextS(text, textConfig) Clay__OpenTextElement(CLAY_STRING(text), textConfig)
#define Text(text, textConfig) Clay__OpenTextElement(text, textConfig)

#define COMPONENT_OPTIONS(...) ((ComponentOptions){__VA_ARGS__})

static Clay_ElementDeclaration boxDefaultOptions = {.layout = {.layoutDirection = CLAY_TOP_TO_BOTTOM, .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)}}};
#define Box(...) CLAY(ParseComponentOptions(COMPONENT_OPTIONS(__VA_ARGS__), boxDefaultOptions))

// Column - A vertical layout container (top to bottom)
static Clay_ElementDeclaration columnDefaultOptions = {.layout = {.layoutDirection = CLAY_TOP_TO_BOTTOM, .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)}}};
#define Column(...) CLAY(ParseComponentOptions(COMPONENT_OPTIONS(__VA_ARGS__), columnDefaultOptions))

// Row - A horizontal layout container (left to right)
static Clay_ElementDeclaration rowDefaultOptions = {.layout = {.layoutDirection = CLAY_LEFT_TO_RIGHT, .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)}}};
#define Row(...) CLAY(ParseComponentOptions(COMPONENT_OPTIONS(__VA_ARGS__), rowDefaultOptions))

static Clay_ElementDeclaration separatorDefaultOptions = {.layout = {.sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)}}};
#define Separator(...) CLAY(ParseComponentOptions(COMPONENT_OPTIONS(__VA_ARGS__), separatorDefaultOptions))

static Clay_ElementDeclaration marginDefaultOptions = {.layout = {.sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)}}};
#define Margin(...) CLAY(ParseComponentOptions(COMPONENT_OPTIONS(__VA_ARGS__), separatorDefaultOptions))

// Colors
#define CHARCOCK (Clay_Color){12, 8, 6, 255}
#define CHARCOAL (Clay_Color){19, 16, 16, 255}
#define CHARCO (Clay_Color){15, 12, 10, 255}
#define WOOD (Clay_Color){84, 58, 20, 255}
#define SAND (Clay_Color){240, 187, 120, 255}
#define CREAM (Clay_Color){245, 230, 210, 255}

// Red Colors
#define RED_50 (Clay_Color){254, 242, 242, 255}
#define RED_100 (Clay_Color){254, 226, 226, 255}
#define RED_200 (Clay_Color){254, 202, 202, 255}
#define RED_300 (Clay_Color){252, 165, 165, 255}
#define RED_400 (Clay_Color){248, 113, 113, 255}
#define RED_500 (Clay_Color){239, 68, 68, 255}
#define RED_600 (Clay_Color){220, 38, 38, 255}
#define RED_700 (Clay_Color){185, 28, 28, 255}
#define RED_800 (Clay_Color){153, 27, 27, 255}
#define RED_900 (Clay_Color){127, 29, 29, 255}
#define RED_950 (Clay_Color){69, 10, 10, 255}

// Orange Colors
#define ORANGE_50 (Clay_Color){255, 247, 237, 255}
#define ORANGE_100 (Clay_Color){255, 237, 213, 255}
#define ORANGE_200 (Clay_Color){254, 215, 170, 255}
#define ORANGE_300 (Clay_Color){253, 186, 116, 255}
#define ORANGE_400 (Clay_Color){251, 146, 60, 255}
#define ORANGE_500 (Clay_Color){249, 115, 22, 255}
#define ORANGE_600 (Clay_Color){234, 88, 12, 255}
#define ORANGE_700 (Clay_Color){194, 65, 12, 255}
#define ORANGE_800 (Clay_Color){154, 52, 18, 255}
#define ORANGE_900 (Clay_Color){124, 45, 18, 255}
#define ORANGE_950 (Clay_Color){67, 20, 7, 255}

// Amber Colors
#define AMBER_50 (Clay_Color){255, 251, 235, 255}
#define AMBER_100 (Clay_Color){254, 243, 199, 255}
#define AMBER_200 (Clay_Color){253, 230, 138, 255}
#define AMBER_300 (Clay_Color){252, 211, 77, 255}
#define AMBER_400 (Clay_Color){251, 191, 36, 255}
#define AMBER_500 (Clay_Color){245, 158, 11, 255}
#define AMBER_600 (Clay_Color){217, 119, 6, 255}
#define AMBER_700 (Clay_Color){180, 83, 9, 255}
#define AMBER_800 (Clay_Color){146, 64, 14, 255}
#define AMBER_900 (Clay_Color){120, 53, 15, 255}
#define AMBER_950 (Clay_Color){69, 26, 3, 255}

// Yellow Colors
#define YELLOW_50 (Clay_Color){254, 252, 232, 255}
#define YELLOW_100 (Clay_Color){254, 249, 195, 255}
#define YELLOW_200 (Clay_Color){254, 240, 138, 255}
#define YELLOW_300 (Clay_Color){253, 224, 71, 255}
#define YELLOW_400 (Clay_Color){250, 204, 21, 255}
#define YELLOW_500 (Clay_Color){234, 179, 8, 255}
#define YELLOW_600 (Clay_Color){202, 138, 4, 255}
#define YELLOW_700 (Clay_Color){161, 98, 7, 255}
#define YELLOW_800 (Clay_Color){133, 77, 14, 255}
#define YELLOW_900 (Clay_Color){113, 63, 18, 255}
#define YELLOW_950 (Clay_Color){66, 32, 6, 255}

// Lime Colors
#define LIME_50 (Clay_Color){247, 254, 231, 255}
#define LIME_100 (Clay_Color){236, 252, 203, 255}
#define LIME_200 (Clay_Color){217, 249, 157, 255}
#define LIME_300 (Clay_Color){190, 242, 100, 255}
#define LIME_400 (Clay_Color){163, 230, 53, 255}
#define LIME_500 (Clay_Color){132, 204, 22, 255}
#define LIME_600 (Clay_Color){101, 163, 13, 255}
#define LIME_700 (Clay_Color){77, 124, 15, 255}
#define LIME_800 (Clay_Color){63, 98, 18, 255}
#define LIME_900 (Clay_Color){54, 83, 20, 255}
#define LIME_950 (Clay_Color){26, 46, 5, 255}

// Green Colors
#define GREEN_50 (Clay_Color){240, 248, 244, 255}
#define GREEN_100 (Clay_Color){220, 245, 231, 255}
#define GREEN_200 (Clay_Color){186, 239, 208, 255}
#define GREEN_300 (Clay_Color){132, 225, 172, 255}
#define GREEN_400 (Clay_Color){72, 205, 128, 255}
#define GREEN_500 (Clay_Color){34, 180, 94, 255}
#define GREEN_600 (Clay_Color){22, 155, 74, 255}
#define GREEN_700 (Clay_Color){21, 123, 61, 255}
#define GREEN_800 (Clay_Color){22, 97, 52, 255}
#define GREEN_900 (Clay_Color){19, 78, 45, 255}
#define GREEN_950 (Clay_Color){5, 42, 22, 255}

// Emerald Colors
#define EMERALD_50 (Clay_Color){236, 253, 245, 255}
#define EMERALD_100 (Clay_Color){209, 250, 229, 255}
#define EMERALD_200 (Clay_Color){167, 243, 208, 255}
#define EMERALD_300 (Clay_Color){110, 231, 183, 255}
#define EMERALD_400 (Clay_Color){52, 211, 153, 255}
#define EMERALD_500 (Clay_Color){16, 185, 129, 255}
#define EMERALD_600 (Clay_Color){5, 150, 105, 255}
#define EMERALD_700 (Clay_Color){4, 120, 87, 255}
#define EMERALD_800 (Clay_Color){6, 95, 70, 255}
#define EMERALD_900 (Clay_Color){6, 78, 59, 255}
#define EMERALD_950 (Clay_Color){2, 44, 34, 255}

// Teal Colors
#define TEAL_50 (Clay_Color){240, 253, 250, 255}
#define TEAL_100 (Clay_Color){204, 251, 241, 255}
#define TEAL_200 (Clay_Color){153, 246, 228, 255}
#define TEAL_300 (Clay_Color){94, 234, 212, 255}
#define TEAL_400 (Clay_Color){45, 212, 191, 255}
#define TEAL_500 (Clay_Color){20, 184, 166, 255}
#define TEAL_600 (Clay_Color){13, 148, 136, 255}
#define TEAL_700 (Clay_Color){15, 118, 110, 255}
#define TEAL_800 (Clay_Color){17, 94, 89, 255}
#define TEAL_900 (Clay_Color){19, 78, 74, 255}
#define TEAL_950 (Clay_Color){4, 47, 46, 255}

// Cyan Colors
#define CYAN_50 (Clay_Color){236, 254, 255, 255}
#define CYAN_100 (Clay_Color){207, 250, 254, 255}
#define CYAN_200 (Clay_Color){165, 243, 252, 255}
#define CYAN_300 (Clay_Color){103, 232, 249, 255}
#define CYAN_400 (Clay_Color){34, 211, 238, 255}
#define CYAN_500 (Clay_Color){6, 182, 212, 255}
#define CYAN_600 (Clay_Color){8, 145, 178, 255}
#define CYAN_700 (Clay_Color){14, 116, 144, 255}
#define CYAN_800 (Clay_Color){21, 94, 117, 255}
#define CYAN_900 (Clay_Color){22, 78, 99, 255}
#define CYAN_950 (Clay_Color){8, 51, 68, 255}

// Sky Colors
#define SKY_50 (Clay_Color){240, 249, 255, 255}
#define SKY_100 (Clay_Color){224, 242, 254, 255}
#define SKY_200 (Clay_Color){186, 230, 253, 255}
#define SKY_300 (Clay_Color){125, 211, 252, 255}
#define SKY_400 (Clay_Color){56, 189, 248, 255}
#define SKY_500 (Clay_Color){14, 165, 233, 255}
#define SKY_600 (Clay_Color){2, 132, 199, 255}
#define SKY_700 (Clay_Color){3, 105, 161, 255}
#define SKY_800 (Clay_Color){7, 89, 133, 255}
#define SKY_900 (Clay_Color){12, 74, 110, 255}
#define SKY_950 (Clay_Color){8, 47, 73, 255}

// Blue Colors
#define BLUE_50 (Clay_Color){239, 246, 255, 255}
#define BLUE_100 (Clay_Color){219, 234, 254, 255}
#define BLUE_200 (Clay_Color){191, 219, 254, 255}
#define BLUE_300 (Clay_Color){147, 197, 253, 255}
#define BLUE_400 (Clay_Color){96, 165, 250, 255}
#define BLUE_500 (Clay_Color){59, 130, 246, 255}
#define BLUE_600 (Clay_Color){37, 99, 235, 255}
#define BLUE_700 (Clay_Color){29, 78, 216, 255}
#define BLUE_800 (Clay_Color){30, 64, 175, 255}
#define BLUE_900 (Clay_Color){30, 58, 138, 255}
#define BLUE_950 (Clay_Color){23, 37, 84, 255}

// Indigo Colors
#define INDIGO_50 (Clay_Color){238, 242, 255, 255}
#define INDIGO_100 (Clay_Color){224, 231, 255, 255}
#define INDIGO_200 (Clay_Color){199, 210, 254, 255}
#define INDIGO_300 (Clay_Color){165, 180, 252, 255}
#define INDIGO_400 (Clay_Color){129, 140, 248, 255}
#define INDIGO_500 (Clay_Color){99, 102, 241, 255}
#define INDIGO_600 (Clay_Color){79, 70, 229, 255}
#define INDIGO_700 (Clay_Color){67, 56, 202, 255}
#define INDIGO_800 (Clay_Color){55, 48, 163, 255}
#define INDIGO_900 (Clay_Color){49, 46, 129, 255}
#define INDIGO_950 (Clay_Color){30, 27, 75, 255}

// Violet Colors
#define VIOLET_50 (Clay_Color){245, 243, 255, 255}
#define VIOLET_100 (Clay_Color){237, 233, 254, 255}
#define VIOLET_200 (Clay_Color){221, 214, 254, 255}
#define VIOLET_300 (Clay_Color){196, 181, 253, 255}
#define VIOLET_400 (Clay_Color){167, 139, 250, 255}
#define VIOLET_500 (Clay_Color){139, 92, 246, 255}
#define VIOLET_600 (Clay_Color){124, 58, 237, 255}
#define VIOLET_700 (Clay_Color){109, 40, 217, 255}
#define VIOLET_800 (Clay_Color){91, 33, 182, 255}
#define VIOLET_900 (Clay_Color){76, 29, 149, 255}
#define VIOLET_950 (Clay_Color){46, 16, 101, 255}

// Purple Colors
#define PURPLE_50 (Clay_Color){253, 244, 255, 255}
#define PURPLE_100 (Clay_Color){250, 232, 255, 255}
#define PURPLE_200 (Clay_Color){245, 208, 254, 255}
#define PURPLE_300 (Clay_Color){240, 171, 252, 255}
#define PURPLE_400 (Clay_Color){232, 121, 249, 255}
#define PURPLE_500 (Clay_Color){217, 70, 239, 255}
#define PURPLE_600 (Clay_Color){192, 38, 211, 255}
#define PURPLE_700 (Clay_Color){162, 28, 175, 255}
#define PURPLE_800 (Clay_Color){134, 25, 143, 255}
#define PURPLE_900 (Clay_Color){112, 26, 117, 255}
#define PURPLE_950 (Clay_Color){74, 4, 78, 255}

// Fuchsia Colors
#define FUCHSIA_50 (Clay_Color){253, 244, 255, 255}
#define FUCHSIA_100 (Clay_Color){250, 232, 255, 255}
#define FUCHSIA_200 (Clay_Color){245, 208, 254, 255}
#define FUCHSIA_300 (Clay_Color){240, 171, 252, 255}
#define FUCHSIA_400 (Clay_Color){232, 121, 249, 255}
#define FUCHSIA_500 (Clay_Color){217, 70, 239, 255}
#define FUCHSIA_600 (Clay_Color){192, 38, 211, 255}
#define FUCHSIA_700 (Clay_Color){162, 28, 175, 255}
#define FUCHSIA_800 (Clay_Color){134, 25, 143, 255}
#define FUCHSIA_900 (Clay_Color){112, 26, 117, 255}
#define FUCHSIA_950 (Clay_Color){74, 4, 78, 255}

// Pink Colors
#define PINK_50 (Clay_Color){253, 242, 248, 255}
#define PINK_100 (Clay_Color){252, 231, 243, 255}
#define PINK_200 (Clay_Color){251, 207, 232, 255}
#define PINK_300 (Clay_Color){249, 168, 212, 255}
#define PINK_400 (Clay_Color){244, 114, 182, 255}
#define PINK_500 (Clay_Color){236, 72, 153, 255}
#define PINK_600 (Clay_Color){219, 39, 119, 255}
#define PINK_700 (Clay_Color){190, 24, 93, 255}
#define PINK_800 (Clay_Color){157, 23, 77, 255}
#define PINK_900 (Clay_Color){131, 24, 67, 255}
#define PINK_950 (Clay_Color){80, 7, 36, 255}

// Rose Colors
#define ROSE_50 (Clay_Color){255, 241, 242, 255}
#define ROSE_100 (Clay_Color){255, 228, 230, 255}
#define ROSE_200 (Clay_Color){254, 205, 211, 255}
#define ROSE_300 (Clay_Color){253, 164, 175, 255}
#define ROSE_400 (Clay_Color){251, 113, 133, 255}
#define ROSE_500 (Clay_Color){244, 63, 94, 255}
#define ROSE_600 (Clay_Color){225, 29, 72, 255}
#define ROSE_700 (Clay_Color){190, 18, 60, 255}
#define ROSE_800 (Clay_Color){159, 18, 57, 255}
#define ROSE_900 (Clay_Color){136, 19, 55, 255}
#define ROSE_950 (Clay_Color){76, 5, 25, 255}

// Slate Colors
#define SLATE_50 (Clay_Color){248, 250, 252, 255}
#define SLATE_100 (Clay_Color){241, 245, 249, 255}
#define SLATE_200 (Clay_Color){226, 232, 240, 255}
#define SLATE_300 (Clay_Color){203, 213, 225, 255}
#define SLATE_400 (Clay_Color){148, 163, 184, 255}
#define SLATE_500 (Clay_Color){100, 116, 139, 255}
#define SLATE_600 (Clay_Color){71, 85, 105, 255}
#define SLATE_700 (Clay_Color){51, 65, 85, 255}
#define SLATE_800 (Clay_Color){30, 41, 59, 255}
#define SLATE_900 (Clay_Color){15, 23, 42, 255}
#define SLATE_950 (Clay_Color){2, 6, 23, 255}

// Gray Colors
#define GRAY_50 (Clay_Color){249, 250, 251, 255}
#define GRAY_100 (Clay_Color){243, 244, 246, 255}
#define GRAY_200 (Clay_Color){229, 231, 235, 255}
#define GRAY_300 (Clay_Color){209, 213, 219, 255}
#define GRAY_400 (Clay_Color){156, 163, 175, 255}
#define GRAY_500 (Clay_Color){107, 114, 128, 255}
#define GRAY_600 (Clay_Color){75, 85, 99, 255}
#define GRAY_700 (Clay_Color){55, 65, 81, 255}
#define GRAY_800 (Clay_Color){31, 41, 55, 255}
#define GRAY_900 (Clay_Color){17, 24, 39, 255}
#define GRAY_950 (Clay_Color){3, 7, 18, 255}

// Zinc Colors
#define ZINC_50 (Clay_Color){250, 250, 250, 255}
#define ZINC_100 (Clay_Color){244, 244, 245, 255}
#define ZINC_200 (Clay_Color){228, 228, 231, 255}
#define ZINC_300 (Clay_Color){212, 212, 216, 255}
#define ZINC_400 (Clay_Color){161, 161, 170, 255}
#define ZINC_500 (Clay_Color){113, 113, 122, 255}
#define ZINC_600 (Clay_Color){82, 82, 91, 255}
#define ZINC_700 (Clay_Color){63, 63, 70, 255}
#define ZINC_800 (Clay_Color){39, 39, 42, 255}
#define ZINC_900 (Clay_Color){24, 24, 27, 255}
#define ZINC_950 (Clay_Color){9, 9, 11, 255}

// Neutral Colors
#define NEUTRAL_50 (Clay_Color){250, 250, 250, 255}
#define NEUTRAL_100 (Clay_Color){245, 245, 245, 255}
#define NEUTRAL_200 (Clay_Color){229, 229, 229, 255}
#define NEUTRAL_300 (Clay_Color){212, 212, 212, 255}
#define NEUTRAL_400 (Clay_Color){163, 163, 163, 255}
#define NEUTRAL_500 (Clay_Color){115, 115, 115, 255}
#define NEUTRAL_600 (Clay_Color){82, 82, 82, 255}
#define NEUTRAL_700 (Clay_Color){64, 64, 64, 255}
#define NEUTRAL_800 (Clay_Color){38, 38, 38, 255}
#define NEUTRAL_900 (Clay_Color){20, 20, 20, 255}
#define NEUTRAL_950 (Clay_Color){10, 10, 10, 255}
#define NEUTRAL_1000 (Clay_Color){6, 6, 6, 255}

// Stone Colors
#define STONE_50 (Clay_Color){250, 250, 249, 255}
#define STONE_100 (Clay_Color){245, 245, 244, 255}
#define STONE_200 (Clay_Color){231, 229, 228, 255}
#define STONE_300 (Clay_Color){214, 211, 209, 255}
#define STONE_400 (Clay_Color){168, 162, 158, 255}
#define STONE_500 (Clay_Color){120, 113, 108, 255}
#define STONE_600 (Clay_Color){87, 83, 78, 255}
#define STONE_700 (Clay_Color){68, 64, 60, 255}
#define STONE_800 (Clay_Color){41, 37, 36, 255}
#define STONE_900 (Clay_Color){28, 25, 23, 255}
#define STONE_950 (Clay_Color){12, 10, 9, 255}

// None Color
#define NONE (Clay_Color){0, 0, 0, 0}

#ifdef RENDERER_IMPLEMENTATION
/*
  Default raylib_renderer.c stuff
  Source: https://github.com/nicbarker/clay/blob/main/renderers/raylib/clay_renderer_raylib.c
*/

// Get a ray trace from the screen position (i.e mouse) within a specific section of the screen
Ray GetScreenToWorldPointWithZDistance(Vector2 position, Camera camera, int screenWidth, int screenHeight, float zDistance) {
  Ray ray = {0};

  // Calculate normalized device coordinates
  // NOTE: y value is negative
  float x = (2.0f * position.x) / (float)screenWidth - 1.0f;
  float y = 1.0f - (2.0f * position.y) / (float)screenHeight;
  float z = 1.0f;

  // Store values in a vector
  Vector3 deviceCoords = {x, y, z};

  // Calculate view matrix from camera look at
  Matrix matView = MatrixLookAt(camera.position, camera.target, camera.up);

  Matrix matProj = MatrixIdentity();

  if (camera.projection == CAMERA_PERSPECTIVE) {
    // Calculate projection matrix from perspective
    matProj = MatrixPerspective(camera.fovy * DEG2RAD, ((double)screenWidth / (double)screenHeight), 0.01f, zDistance);
  } else if (camera.projection == CAMERA_ORTHOGRAPHIC) {
    double aspect = (double)screenWidth / (double)screenHeight;
    double top = camera.fovy / 2.0;
    double right = top * aspect;

    // Calculate projection matrix from orthographic
    matProj = MatrixOrtho(-right, right, -top, top, 0.01, 1000.0);
  }

  // Unproject far/near points
  Vector3 nearPoint = Vector3Unproject((Vector3){deviceCoords.x, deviceCoords.y, 0.0f}, matProj, matView);
  Vector3 farPoint = Vector3Unproject((Vector3){deviceCoords.x, deviceCoords.y, 1.0f}, matProj, matView);

  // Calculate normalized direction vector
  Vector3 direction = Vector3Normalize(Vector3Subtract(farPoint, nearPoint));

  ray.position = farPoint;

  // Apply calculated vectors to ray
  ray.direction = direction;

  return ray;
}

static inline Clay_Dimensions Raylib_MeasureText(Clay_StringSlice text, Clay_TextElementConfig *config, void *userData) {
  // Measure string size for Font
  Clay_Dimensions textSize = {0};

  float maxTextWidth = 0.0f;
  float lineTextWidth = 0;

  float textHeight = config->fontSize;
  Font *fonts = (Font *)userData;
  Font fontToUse = fonts[config->fontId];
  // Font failed to load, likely the fonts are in the wrong place relative to the execution dir.
  // RayLib ships with a default font, so we can continue with that built in one.
  if (!fontToUse.glyphs) {
    fontToUse = GetFontDefault();
  }

  float scaleFactor = config->fontSize / (float)fontToUse.baseSize;

  for (int i = 0; i < text.length; ++i) {
    if (text.chars[i] == '\n') {
      maxTextWidth = fmax(maxTextWidth, lineTextWidth);
      lineTextWidth = 0;
      continue;
    }
    int index = text.chars[i] - 32;
    if (fontToUse.glyphs[index].advanceX != 0) lineTextWidth += fontToUse.glyphs[index].advanceX;
    else lineTextWidth += (fontToUse.recs[index].width + fontToUse.glyphs[index].offsetX);
  }

  maxTextWidth = fmax(maxTextWidth, lineTextWidth);

  textSize.width = maxTextWidth * scaleFactor;
  textSize.height = textHeight;

  return textSize;
}

void Clay_Raylib_Initialize(int width, int height, const char *title, unsigned int flags) {
  SetConfigFlags(flags);
  InitWindow(width, height, title);
}

void Clay_Raylib_Render(Clay_RenderCommandArray renderCommands, Font *fonts) {
  for (int j = 0; j < renderCommands.length; j++) {
    Clay_RenderCommand *renderCommand = Clay_RenderCommandArray_Get(&renderCommands, j);
    Clay_BoundingBox boundingBox = renderCommand->boundingBox;
    switch (renderCommand->commandType) {
    case CLAY_RENDER_COMMAND_TYPE_TEXT: {
      // Raylib uses standard C strings so isn't compatible with cheap slices, we need to clone the string to append null terminator
      Clay_TextRenderData *textData = &renderCommand->renderData.text;
      char *cloned = (char *)malloc(textData->stringContents.length + 1);
      memcpy(cloned, textData->stringContents.chars, textData->stringContents.length);
      cloned[textData->stringContents.length] = '\0';
      Font fontToUse = fonts[textData->fontId];
      DrawTextEx(fontToUse, cloned, (Vector2){boundingBox.x, boundingBox.y}, (float)textData->fontSize, (float)textData->letterSpacing, CLAY_COLOR_TO_RAYLIB_COLOR(textData->textColor));
      free(cloned);
      break;
    }
    case CLAY_RENDER_COMMAND_TYPE_IMAGE: {
      Texture2D imageTexture = *(Texture2D *)renderCommand->renderData.image.imageData;
      Clay_Color tintColor = renderCommand->renderData.image.backgroundColor;
      if (tintColor.r == 0 && tintColor.g == 0 && tintColor.b == 0 && tintColor.a == 0) {
        tintColor = (Clay_Color){255, 255, 255, 255};
      }
      DrawTextureEx(imageTexture, (Vector2){boundingBox.x, boundingBox.y}, 0, boundingBox.width / (float)imageTexture.width, CLAY_COLOR_TO_RAYLIB_COLOR(tintColor));
      break;
    }
    case CLAY_RENDER_COMMAND_TYPE_SCISSOR_START: {
      BeginScissorMode((int)roundf(boundingBox.x), (int)roundf(boundingBox.y), (int)roundf(boundingBox.width), (int)roundf(boundingBox.height));
      break;
    }
    case CLAY_RENDER_COMMAND_TYPE_SCISSOR_END: {
      EndScissorMode();
      break;
    }
    case CLAY_RENDER_COMMAND_TYPE_RECTANGLE: {
      Clay_RectangleRenderData *config = &renderCommand->renderData.rectangle;
      if (config->cornerRadius.topLeft > 0) {
        float radius = (config->cornerRadius.topLeft * 2) / (float)((boundingBox.width > boundingBox.height) ? boundingBox.height : boundingBox.width);
        DrawRectangleRounded((Rectangle){boundingBox.x, boundingBox.y, boundingBox.width, boundingBox.height}, radius, 8, CLAY_COLOR_TO_RAYLIB_COLOR(config->backgroundColor));
      } else {
        DrawRectangle(boundingBox.x, boundingBox.y, boundingBox.width, boundingBox.height, CLAY_COLOR_TO_RAYLIB_COLOR(config->backgroundColor));
      }
      break;
    }
    case CLAY_RENDER_COMMAND_TYPE_BORDER: {
      Clay_BorderRenderData *config = &renderCommand->renderData.border;
      // Left border
      if (config->width.left > 0) {
        DrawRectangle((int)roundf(boundingBox.x),
                      (int)roundf(boundingBox.y + config->cornerRadius.topLeft),
                      (int)config->width.left,
                      (int)roundf(boundingBox.height - config->cornerRadius.topLeft - config->cornerRadius.bottomLeft),
                      CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
      }
      // Right border
      if (config->width.right > 0) {
        DrawRectangle((int)roundf(boundingBox.x + boundingBox.width - config->width.right),
                      (int)roundf(boundingBox.y + config->cornerRadius.topRight),
                      (int)config->width.right,
                      (int)roundf(boundingBox.height - config->cornerRadius.topRight - config->cornerRadius.bottomRight),
                      CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
      }
      // Top border
      if (config->width.top > 0) {
        DrawRectangle((int)roundf(boundingBox.x + config->cornerRadius.topLeft),
                      (int)roundf(boundingBox.y),
                      (int)roundf(boundingBox.width - config->cornerRadius.topLeft - config->cornerRadius.topRight),
                      (int)config->width.top,
                      CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
      }
      // Bottom border
      if (config->width.bottom > 0) {
        DrawRectangle((int)roundf(boundingBox.x + config->cornerRadius.bottomLeft),
                      (int)roundf(boundingBox.y + boundingBox.height - config->width.bottom),
                      (int)roundf(boundingBox.width - config->cornerRadius.bottomLeft - config->cornerRadius.bottomRight),
                      (int)config->width.bottom,
                      CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
      }
      if (config->cornerRadius.topLeft > 0) {
        DrawRing((Vector2){roundf(boundingBox.x + config->cornerRadius.topLeft), roundf(boundingBox.y + config->cornerRadius.topLeft)},
                 roundf(config->cornerRadius.topLeft - config->width.top),
                 config->cornerRadius.topLeft,
                 180,
                 270,
                 10,
                 CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
      }
      if (config->cornerRadius.topRight > 0) {
        DrawRing((Vector2){roundf(boundingBox.x + boundingBox.width - config->cornerRadius.topRight), roundf(boundingBox.y + config->cornerRadius.topRight)},
                 roundf(config->cornerRadius.topRight - config->width.top),
                 config->cornerRadius.topRight,
                 270,
                 360,
                 10,
                 CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
      }
      if (config->cornerRadius.bottomLeft > 0) {
        DrawRing((Vector2){roundf(boundingBox.x + config->cornerRadius.bottomLeft), roundf(boundingBox.y + boundingBox.height - config->cornerRadius.bottomLeft)},
                 roundf(config->cornerRadius.bottomLeft - config->width.top),
                 config->cornerRadius.bottomLeft,
                 90,
                 180,
                 10,
                 CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
      }
      if (config->cornerRadius.bottomRight > 0) {
        DrawRing((Vector2){roundf(boundingBox.x + boundingBox.width - config->cornerRadius.bottomRight), roundf(boundingBox.y + boundingBox.height - config->cornerRadius.bottomRight)},
                 roundf(config->cornerRadius.bottomRight - config->width.bottom),
                 config->cornerRadius.bottomRight,
                 0.1,
                 90,
                 10,
                 CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
      }
      break;
    }
    case CLAY_RENDER_COMMAND_TYPE_CUSTOM: {
      Clay_CustomRenderData *config = &renderCommand->renderData.custom;
      CustomLayoutElement *customElement = (CustomLayoutElement *)config->customData;
      if (!customElement) continue;
      switch (customElement->type) {
      case CUSTOM_LAYOUT_ELEMENT_TYPE_3D_MODEL: {
        Clay_BoundingBox rootBox = renderCommands.internalArray[0].boundingBox;
        float scaleValue = CLAY__MIN(CLAY__MIN(1, 768 / rootBox.height) * CLAY__MAX(1, rootBox.width / 1024), 1.5f);
        Ray positionRay = GetScreenToWorldPointWithZDistance((Vector2){renderCommand->boundingBox.x + renderCommand->boundingBox.width / 2, renderCommand->boundingBox.y + (renderCommand->boundingBox.height / 2) + 20},
                                                             Raylib_camera,
                                                             (int)roundf(rootBox.width),
                                                             (int)roundf(rootBox.height),
                                                             140);
        BeginMode3D(Raylib_camera);
        DrawModel(customElement->customData.model.model, positionRay.position, customElement->customData.model.scale * scaleValue, WHITE); // Draw 3d model with texture
        EndMode3D();
        break;
      }
      default:
        break;
      }
      break;
    }
    default: {
      printf("Error: unhandled render command.");
      exit(1);
    }
    }
  }
}

Clay_String toClayString(char *str) {
  return (Clay_String){.length = strlen(str), .chars = str};
}

/* Our renderer.h specifics */
Renderer renderer = {0};
void HandleClayErrors(Clay_ErrorData errorData) {
  printf("%s", errorData.errorText.chars);

  if (errorData.errorType == CLAY_ERROR_TYPE_ELEMENTS_CAPACITY_EXCEEDED) {
    renderer.reinitialize = true;
    Clay_SetMaxElementCount(Clay_GetMaxElementCount() * 2);
    return;
  }

  if (errorData.errorType == CLAY_ERROR_TYPE_TEXT_MEASUREMENT_CAPACITY_EXCEEDED) {
    renderer.reinitialize = true;
    Clay_SetMaxMeasureTextCacheWordCount(Clay_GetMaxMeasureTextCacheWordCount() * 2);
    return;
  }
}

static void initDraw() {
  if (IsKeyPressed(KEY_F2)) {
    renderer.debugEnabled = !renderer.debugEnabled;
    Clay_SetDebugModeEnabled(renderer.debugEnabled);
  }

  Clay_Vector2 mousePosition = RAYLIB_VECTOR2_TO_CLAY_VECTOR2(GetMousePosition());
  Clay_SetPointerState(mousePosition, IsMouseButtonDown(0));
  Clay_SetLayoutDimensions((Clay_Dimensions){(float)GetScreenWidth(), (float)GetScreenHeight()});

  Vector2 mouseWheelDelta = GetMouseWheelMoveV();
  float mouseWheelX = mouseWheelDelta.x;
  float mouseWheelY = mouseWheelDelta.y;
  Clay_UpdateScrollContainers(true, (Clay_Vector2){mouseWheelX, mouseWheelY}, GetFrameTime());
}

void ScrollContainerByY(char *containerName, float deltaY) {
  Clay_ScrollContainerData scrollContainerData = Clay_GetScrollContainerData(Clay__HashString(toClayString(containerName), 0, 0));
  float newScrollY = scrollContainerData.scrollPosition->y + deltaY;
  float minScrollY = -fmaxf(0, scrollContainerData.contentDimensions.height - scrollContainerData.scrollContainerDimensions.height);
  float newValue = fminf(0, fmaxf(newScrollY, minScrollY));
  if (newValue == 0) {
    printf("New value is higher min value\n");
    printf("deltaY: %f, minPos: %f, newPos: %f", deltaY, minScrollY, fminf(0, fmaxf(newScrollY, minScrollY)));
  }
  scrollContainerData.scrollPosition->y = newValue;
}

void ScrollContainerTop(char *containerName) {
  Clay_ScrollContainerData scrollContainerData = Clay_GetScrollContainerData(Clay__HashString(toClayString(containerName), 0, 0));
  scrollContainerData.scrollPosition->y = 0;
}

void ScrollContainerBottom(char *containerName) {
  Clay_ScrollContainerData scrollContainerData = Clay_GetScrollContainerData(Clay__HashString(toClayString(containerName), 0, 0));
  float minScrollY = -fmaxf(0, scrollContainerData.contentDimensions.height - scrollContainerData.scrollContainerDimensions.height);
  scrollContainerData.scrollPosition->y = minScrollY;
}

void ScrollContainerByX(char *containerName, float deltaX) {
  Clay_ScrollContainerData scrollContainerData = Clay_GetScrollContainerData(Clay__HashString(toClayString(containerName), 0, 0));
  float newScrollX = scrollContainerData.scrollPosition->x + deltaX;
  float minScrollX = -fmaxf(0, scrollContainerData.contentDimensions.width - scrollContainerData.scrollContainerDimensions.width);
  scrollContainerData.scrollPosition->x = fminf(0, fmaxf(newScrollX, minScrollX));
}

Clay_String s(const char *msg) {
  return (Clay_String){
      .length = strlen(msg),
      .chars = msg,
  };
}

void RenderSetup(RenderOptions options, Callback updateCallback, Callback drawCallback) {
  renderer.totalMemorySize = Clay_MinMemorySize();
  renderer.clayMemory = Clay_CreateArenaWithCapacityAndMemory(renderer.totalMemorySize, malloc(renderer.totalMemorySize));
  Clay_Initialize(renderer.clayMemory, (Clay_Dimensions){(float)GetScreenWidth(), (float)GetScreenHeight()}, (Clay_ErrorHandler){HandleClayErrors, 0});
  Clay_Raylib_Initialize(options.width, options.height, options.windowName, FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT);

  renderer.fonts[FONT_18] = LoadFontEx(options.fontPath, 18, 0, 250);
  SetTextureFilter(renderer.fonts[FONT_18].texture, TEXTURE_FILTER_BILINEAR);
  renderer.fonts[FONT_20] = LoadFontEx(options.fontPath, 20, 0, 250);
  SetTextureFilter(renderer.fonts[FONT_20].texture, TEXTURE_FILTER_BILINEAR);
  renderer.fonts[FONT_22] = LoadFontEx(options.fontPath, 22, 0, 250);
  SetTextureFilter(renderer.fonts[FONT_22].texture, TEXTURE_FILTER_BILINEAR);
  renderer.fonts[FONT_24] = LoadFontEx(options.fontPath, 24, 0, 250);
  SetTextureFilter(renderer.fonts[FONT_24].texture, TEXTURE_FILTER_BILINEAR);

  // GenTextureMipmaps(&renderer.font[FONT_24].texture);
  Clay_SetMeasureTextFunction(Raylib_MeasureText, &renderer.fonts);
  while (!renderer.shouldClose) {
    if (IsKeyPressed(KEY_ESCAPE) || WindowShouldClose()) renderer.shouldClose = true;

    if (renderer.reinitialize) {
      Clay_SetMaxElementCount(8192);
      renderer.totalMemorySize = Clay_MinMemorySize();
      renderer.clayMemory = Clay_CreateArenaWithCapacityAndMemory(renderer.totalMemorySize, malloc(renderer.totalMemorySize));
      Clay_Initialize(renderer.clayMemory, (Clay_Dimensions){(float)GetScreenWidth(), (float)GetScreenHeight()}, (Clay_ErrorHandler){HandleClayErrors, 0});
      renderer.reinitialize = false;
    }

    initDraw();
    updateCallback();
    drawCallback();
  }

  CloseWindow();
}

Clay_String F(Arena *arena, const char *format, ...) {
  // Get formatted string size
  va_list args;
  va_start(args, format);
  size_t size = vsnprintf(NULL, 0, format, args) + 1; // +1 for null terminator
  va_end(args);

  // Allocate based on size
  char *buffer = (char *)ArenaAlloc(arena, size);
  va_start(args, format);
  vsnprintf(buffer, size, format, args);
  va_end(args);

  return (Clay_String){.length = size - 1, .chars = buffer};
}

static Clay_ElementDeclaration ParseComponentOptions(ComponentOptions options, Clay_ElementDeclaration defaultOptions) {
  Clay_ElementDeclaration result = defaultOptions;

  // Misc
  {
    if (options.id) {
      result.id = Clay__HashString(toClayString(options.id), 0, 0);
    }

    if (options.bg.r || options.bg.g || options.bg.b || options.bg.a) {
      result.backgroundColor = options.bg;
    }

    if (options.gap) {
      result.layout.childGap = options.gap;
    }
  }

  // Padding parse
  {
    if (options.p) {
      result.layout.padding.top = options.p;
      result.layout.padding.bottom = options.p;
      result.layout.padding.left = options.p;
      result.layout.padding.right = options.p;
    }

    if (options.px) {
      result.layout.padding.left = options.px;
      result.layout.padding.right = options.px;
    }

    if (options.py) {
      result.layout.padding.top = options.py;
      result.layout.padding.bottom = options.py;
    }

    if (options.pt) {
      result.layout.padding.top = options.pt;
    }

    if (options.pb) {
      result.layout.padding.bottom = options.pb;
    }

    if (options.pl) {
      result.layout.padding.left = options.pl;
    }

    if (options.pr) {
      result.layout.padding.right = options.pr;
    }
  }

  // Align parse
  {
    char firstChar = options.align[0];
    char secondChar = options.align[1];
    if (firstChar == 't') {
      result.layout.childAlignment.y = CLAY_ALIGN_Y_TOP;
    }

    if (firstChar == 'c') {
      result.layout.childAlignment.y = CLAY_ALIGN_Y_CENTER;
    }

    if (firstChar == 'b') {
      result.layout.childAlignment.y = CLAY_ALIGN_Y_BOTTOM;
    }

    if (secondChar == 'l') {
      result.layout.childAlignment.x = CLAY_ALIGN_X_LEFT;
    }

    if (secondChar == 'c') {
      result.layout.childAlignment.x = CLAY_ALIGN_X_CENTER;
    }

    if (secondChar == 'r') {
      result.layout.childAlignment.x = CLAY_ALIGN_X_RIGHT;
    }
  }

  // Scroll parse
  {
    char firstChar = options.scroll[0];
    if (firstChar == 'v') {
      result.scroll = (Clay_ScrollElementConfig){.vertical = true};
    }

    if (firstChar == 'h') {
      result.scroll = (Clay_ScrollElementConfig){.horizontal = true};
    }

    if (firstChar == 'b') {
      result.scroll = (Clay_ScrollElementConfig){.vertical = true, .horizontal = true};
    }
  }

  // Border radius parse
  {
    if (strcmp(options.borderRadius, "") != 0) {
      size_t stringLen = strnlen(options.borderRadius, 6);
      char *delimiter = strrchr(options.borderRadius, '-');

      assert(delimiter != NULL && "Border radius format is {direction}-{size} ex: t-sm");

      size_t directionLen = delimiter - options.borderRadius;
      size_t sizeLen = stringLen - (directionLen + 1);

      char directionStr[3];
      memcpy(directionStr, options.borderRadius, directionLen);
      directionStr[directionLen] = '\0';

      char sizeStr[4];
      memcpy(sizeStr, delimiter + 1, sizeLen);
      sizeStr[sizeLen] = '\0';

      float size = 0;
      if (strcmp(sizeStr, "xs") == 0) {
        size = 2;
      }

      if (strcmp(sizeStr, "sm") == 0) {
        size = 4;
      }

      if (strcmp(sizeStr, "md") == 0) {
        size = 6;
      }

      if (strcmp(sizeStr, "lg") == 0) {
        size = 8;
      }

      if (strcmp(sizeStr, "xl") == 0) {
        size = 12;
      }

      if (strcmp(sizeStr, "2xl") == 0) {
        size = 16;
      }

      if (strcmp(sizeStr, "3xl") == 0) {
        size = 24;
      }

      // Corner Radius
      if (strcmp(directionStr, "t") == 0) {
        result.cornerRadius = (Clay_CornerRadius){.topLeft = size, .topRight = size};
      }

      if (strcmp(directionStr, "b") == 0) {
        result.cornerRadius = (Clay_CornerRadius){.bottomLeft = size, .bottomRight = size};
      }

      if (strcmp(directionStr, "l") == 0) {
        result.cornerRadius = (Clay_CornerRadius){.topLeft = size, .bottomLeft = size};
      }

      if (strcmp(directionStr, "r") == 0) {
        result.cornerRadius = (Clay_CornerRadius){.topRight = size, .bottomRight = size};
      }

      if (strcmp(directionStr, "tl") == 0) {
        result.cornerRadius = (Clay_CornerRadius){.topLeft = size};
      }

      if (strcmp(directionStr, "tr") == 0) {
        result.cornerRadius = (Clay_CornerRadius){.topRight = size};
      }

      if (strcmp(directionStr, "bl") == 0) {
        result.cornerRadius = (CLAY__INIT(Clay_CornerRadius){.bottomLeft = size});
      }

      if (strcmp(directionStr, "br") == 0) {
        result.cornerRadius = (CLAY__INIT(Clay_CornerRadius){.bottomRight = size});
      }

      if (strcmp(directionStr, "a") == 0) {
        result.cornerRadius = CLAY_CORNER_RADIUS(size);
      }
    }
  }

  // Border width parsing
  {
    if (options.border.color.a != 0) {
      size_t stringLen = strnlen(options.border.width, 6);
      char *delimiter = strrchr(options.border.width, '-');

      assert(delimiter != NULL && "Border format is {width}-{size} ex: t-50");

      size_t widthLen = delimiter - options.border.width;
      size_t sizeLen = stringLen - (widthLen + 1);

      char directionStr = options.border.width[0];

      char widthStr[4];
      memcpy(widthStr, delimiter + 1, sizeLen);
      widthStr[sizeLen] = '\0';

      if (directionStr == 't') {
        result.border.width = (Clay_BorderWidth){.top = atof(widthStr)};
      }

      if (directionStr == 'b') {
        result.border.width = (Clay_BorderWidth){.bottom = atof(widthStr)};
      }

      if (directionStr == 'l') {
        result.border.width = (Clay_BorderWidth){.left = atof(widthStr)};
      }

      if (directionStr == 'r') {
        result.border.width = (Clay_BorderWidth){.right = atof(widthStr)};
      }

      if (directionStr == 'a') {
        result.border.width = (Clay_BorderWidth){.top = atof(widthStr), .bottom = atof(widthStr), .left = atof(widthStr), .right = atof(widthStr)};
      }

      result.border.color = options.border.color;
    }
  }

  // Sizing
  {
    if (options.h) {
      size_t stringLen = strlen(options.h);
      char *delimiter = strrchr(options.h, '-');
      assert(delimiter != NULL && "Height or width format is {typing}-{size} ex: fit-0");

      size_t typeLength = delimiter - options.h;

      char sizeString[10] = {0};
      size_t sizeLen = stringLen - (typeLength + 1);
      assert(sizeLen < sizeof(sizeString) && "Size string too long for buffer");

      memcpy(sizeString, delimiter + 1, sizeLen);

      assert(typeLength >= 3 && "Unknown sizing type");

      if (typeLength == 3) { // "fit"
        result.layout.sizing.height = CLAY_SIZING_FIT(atof(sizeString));
      }

      if (typeLength == 4) { // "grow"
        result.layout.sizing.height = CLAY_SIZING_GROW(atof(sizeString));
      }

      if (typeLength == 5) { // "fixed"
        result.layout.sizing.height = CLAY_SIZING_FIXED(atof(sizeString));
      }

      if (typeLength == 7) { // "percent"
        result.layout.sizing.height = CLAY_SIZING_PERCENT(atof(sizeString));
      }
    }

    if (options.w) {
      size_t stringLen = strlen(options.w);
      char *delimiter = strrchr(options.w, '-');
      assert(delimiter != NULL && "Height or width format is {typing}-{size} ex: fit-0");

      size_t typeLength = delimiter - options.w;

      char sizeString[10] = {0};
      size_t sizeLen = stringLen - (typeLength + 1);
      assert(sizeLen < sizeof(sizeString) && "Size string too long for buffer");

      memcpy(sizeString, delimiter + 1, sizeLen);

      assert(typeLength >= 3 && "Unknown sizinng type");

      if (typeLength == 3) { // "fit"
        result.layout.sizing.width = CLAY_SIZING_FIT(atof(sizeString));
      }

      if (typeLength == 4) { // "grow"
        result.layout.sizing.width = CLAY_SIZING_GROW(atof(sizeString));
      }

      if (typeLength == 5) { // "fixed"
        result.layout.sizing.width = CLAY_SIZING_FIXED(atof(sizeString));
      }

      if (typeLength == 7) { // "percent"
        result.layout.sizing.width = CLAY_SIZING_PERCENT(atof(sizeString));
      }
    }
  }
  return result;
}

/* Arena Implementation, inspired from:
   https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/
*/
static intptr_t alignForward(const intptr_t ptr) {
  intptr_t p, a, modulo;

  p = ptr;
  a = (intptr_t)DEFAULT_ALIGNMENT;
  // Same as (p % a) but faster as 'a' is a power of two
  modulo = p & (a - 1);

  if (modulo != 0) {
    // If 'p' address is not aligned, push the address to the
    // next value which is aligned
    p += a - modulo;
  }
  return p;
}

void *ArenaAlloc(Arena *a, const size_t size) {
  // Align 'currPtr' forward to the specified alignment
  intptr_t currPtr = (intptr_t)a->buffer + (intptr_t)a->currOffset;
  intptr_t offset = alignForward(currPtr);
  offset -= (intptr_t)a->buffer; // Change to relative offset

  assert(offset + size <= a->bufferLength && "Arena ran out of space left");

  void *ptr = &a->buffer[offset];
  a->prevOffset = offset;
  a->currOffset = offset + size;

  // Zero new memory by default
  memset(ptr, 0, size);
  return ptr;
}

void ArenaFree(Arena *arena) {
  free(arena->buffer);
}

void ArenaReset(Arena *arena) {
  arena->currOffset = 0;
}

Arena ArenaInit(size_t size) {
  return (Arena){
      .buffer = (int8_t *)malloc(size),
      .bufferLength = size,
      .prevOffset = 0,
      .currOffset = 0,
  };
}
#endif
