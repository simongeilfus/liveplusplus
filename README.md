# liveplusplus

[Molecular Matters's Live++](https://liveplusplus.tech/) is really easy to use already but this small wrapper makes it a little bit easier. CMake will take care of downloading the library binaries and define a pre-processor variable to help locate them when initializing Live++. 

The most basic use is to initialize as soon as you can, and then use a sync point at the start of your frame:

```cpp
#include "lxx/lxx.h"

void init()
{
#if defined(LPP_ENABLED)
    lxx::initDefaultAgent();
#endif
}
```

A synchronized agent can created for more flexibility:

```cpp
#include "lxx/lxx.h"

void init()
{
#if defined(LPP_ENABLED)
    lxx::initSynchronizedAgent();
#endif
}

void render()
{
#if defined(LPP_ENABLED)
    if( lxx::wantsReload() ) {
        lxx::reload();
    }
#endif
}
```

A path to a previously created .json file can be provided to load project preferences:

```cpp
#if defined(LPP_ENABLED)
    fs::path lxxPrefs = getAssetPath( "livexx_prefs.json" );
    if( fs::exists( lxxPrefs ) ) lxx::initDefaultAgent( lxxPrefs );
    else lxx::initDefaultAgent();
#endif
```

You can specify what options to customize at initialization time :

```cpp
lpp::LppProjectPreferences preferences = {
    .exceptionHandler = {
        .isEnabled = true,
        .order = 0
    },
    .continuousCompilation = {
        .directory = L"Directory/To/Watch",
        .timeout = 100,
        .isEnabled = true
    }
};

lxx::initDefaultAgent( &preferences );
```


