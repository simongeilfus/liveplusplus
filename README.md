# liveplusplus

[Molecular Matters's Live++](https://liveplusplus.tech/) is really easy to use already but this small wrapper makes it even more easy to integrate to a new project. CMake will take care of downloading the library binaries and define a preprocessor variable to help locate them when initializing Live++.

The most basic use is to initialize as soon as you can, and then use a sync point at the start of your frame:

```cpp
#include "liveplusplus/liveplusplus.h"

void init()
{
    LivePlusPlus::initialize();
}

void render()
{
    LivePlusPlus::syncPoint();
}
```

You can specify what options to customize at initialization time :

```cpp
LivePlusPlus::initialize( LivePlusPlus::Settings()
	.initialWindowMode( LivePlusPlus::WindowMode::Minimized )
	.receiveFocusOnRecompile( LivePlusPlus::FocusMode::Never )
	.enableContinuousCompilation( false )
	.additionalCompilerOptions( L"-DLPP_COMPILED" )
);
```

Or request for a callback to be called after a patch has been applied:

```cpp
LivePlusPlus::instance().addPostPatchFn( [this]() { /* do something */ } );
```



