# liveplusplus

[Molecular Matters's Live++](https://liveplusplus.tech/) is really easy to use already but this small wrapper makes it a little bit easier. CMake will take care of downloading the library binaries and define a pre-processor variable to help locate them when initializing Live++. 

The most basic use is to initialize as soon as you can, and then use a sync point at the start of your frame:

```cpp
#include "liveplusplus/liveplusplus.h"

void init()
{
    lxx::initialize();
}

void render()
{
    lxx::syncPoint();
}
```

You can specify what options to customize at initialization time :

```cpp
lxx::initialize( lxx::Settings()
	.initialWindowMode( lxx::WindowMode::Minimized )
	.receiveFocusOnRecompile( lxx::FocusMode::Never )
	.enableContinuousCompilation( true )
	.additionalCompilerOptions( L"-DLPP_COMPILED" )
);
```

Or request for a callback to be called after a patch has been applied:

```cpp
lxx::instance().addPostPatchFn( [this]() { /* do something */ } );
```



