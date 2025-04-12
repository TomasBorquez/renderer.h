# About:
This is a single header library for `clay.h`, it contains some renderer abstractions and different
components as well as enhancements and bugfixes to clay itself.

## Features:

- Colors - Color MACROS `Clay_Color`, usage is simple, you type the name, ex: `GREEN` and add the intensity `GREEN_500`, they go from 50 to 950.
- Components - Such as `Column` for columns, `Row` for rows, `Separator` to add space between elements, `Box` for general purpose, and more...
- Text - MACROS for adding text both `TextS()` and `Text()`, you can also format text with `F()`, which works the same as sprintf but with arenas.
- And more - Like renderer abstractions, automatic font loading, ...

## Usage:
A good way to keep this updated is by using git submodules, you can add `renderer.h` like this:

```bash
git submodule add https://github.com/TomasBorquez/renderer.h.git ./vendor/renderer
```

This will add `renderer.h` to `./vendor`, then you can include it from there or add a `-I"./vendor/renderer"` flag when compiling and importing like:

```c 
#define CLAY_IMPLEMENTATION
#include "clay.h"
#define RENDERER_IMPLEMENTATION
#include "renderer.h"
```

And for keeping it updated you can:

```C 
git submodule update --init
```

Clay is of course required and in `main.c` or whatever your entrypoint is, you include `renderer.h` and define the macro
`RENDERER_IMPLEMENTATION` which adds the implementations for the renderer.

```c 
int main() {
  RenderOptions options = {
      .width = 1400,
      .height = 900,
      .windowName = "Example",
      .fontPath = "./resources/ComicMono.ttf",
  };
  RenderSetup(options, update, draw);
}
```

Add this setup on your main function.

```c
Clay_RenderCommandArray CreateLayout(void) {
  Clay_BeginLayout();
  {
    Clay_TextElementConfig *textConfig = CLAY_TEXT_CONFIG({.fontId = FONT_24, .fontSize = 24, .textColor = CREAM});
    Box(.id = "Body", .p = 10, .w = "grow-0", .h = "grow-0", .bg = NEUTRAL_950) {
      TextS("Test", textConfig);
    }
  }
  return Clay_EndLayout();
}

void draw() {
  Clay_RenderCommandArray renderCommands = CreateLayout();

  BeginDrawing();
  {
    Clay_Raylib_Render(renderCommands, renderer.fonts);
  }
  EndDrawing();

  ArenaReset(&state.arena);
}

void update() {
  // Whatever you have to update
}
```

And define `draw`, `update` and create your layout. For a full example you can check my [assembly debugger](https://github.com/TomasBorquez/assembly-debugger).

# TODOS:
- [x] Add align, tl, tc, tr, etc.
- [x] Border radius.
- [x] Move color to renderers add all the colors from 50-950
- [x] Scroll in general
- [x] Border
- [x] Fix font height size
- [ ] Antialiasing on border radius
- [ ] Create function that selects fontId based on fontSize, load 8-72 to avoid blurriness
- [ ] ^ Better way of creating `Clay_TextElementConfig`
- [ ] Figure out how to get correct textHeight
- [ ] Follow tailwind guidelines better

## When Available:
- [ ] Individual CornerRadius when available
- [ ] Reverse when available
- [ ] Margin when available

