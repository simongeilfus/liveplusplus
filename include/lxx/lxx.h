/*
Copyright 2020 Simon Geilfus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
and associated documentation files (the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, 
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial 
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT 
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES 
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#pragma once

#include <filesystem>
#include <functional>
#include <optional>

#if ! defined( LPP_PATH )
	#define LPP_PATH "OVERRIDE_IF_NOT_USING_CMAKE"
#endif

namespace lpp {
struct LppDefaultAgent;
struct LppSynchronizedAgent;
struct LppProjectPreferences;
}

class lxx {
public:
	//! Initialize the global instance with optional Project Preferences
	static void initDefaultAgent( const lpp::LppProjectPreferences* projectPreferences = nullptr, const std::filesystem::path &lppPath = LPP_PATH );
	//! Initialize the global instance with a path to the preferences .json file
	static void initDefaultAgent( const std::filesystem::path &projectPreferencesPath, const std::filesystem::path &lppPath = LPP_PATH );
	//! Initialize the global instance with optional Project Preferences
	static void initSynchronizedAgent( const lpp::LppProjectPreferences* projectPreferences = nullptr, const std::filesystem::path &lppPath = LPP_PATH );
	//! Initialize the global instance with a path to the preferences .json file
	static void initSynchronizedAgent( const std::filesystem::path &projectPreferencesPath, const std::filesystem::path &lppPath = LPP_PATH );
    //! Listens for hot-reload events and optionnal synchronizes with compilation
	static bool wantsReload( bool synchronizeWithCompilation = false );
    //! Triggers a hot-reload
	static void reload( bool waitUntilChangesAreApplied = true );
	//! Listens for hot-restart requests
	static bool wantsRestart();
	//! Triggers a hot-restart. use LppSynchronizedAgent::Restart for more options.
	static void restart();
	//! Returns the global default agent. expects initDefaultAgent to have been called.
	static lpp::LppDefaultAgent& defaultAgent();
	//! Returns the global agent. expects initSynchronizedAgent to have been called.
	static lpp::LppSynchronizedAgent& synchronizedAgent();
private:
	lxx();
	~lxx();
	static lxx& instance();
	std::unique_ptr<lpp::LppSynchronizedAgent> mSynchronizedAgent;
	std::unique_ptr<lpp::LppDefaultAgent> mDefaultAgent;
};