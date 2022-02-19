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
	#define LPP_PATH "D:\\LivePP"
#endif

class LivePlusPlus {
public:
	enum class WindowMode { Normal, Minimized, Maximized };
	enum class FocusMode { OnError, OnSuccess, OnShortcut, Never };

	class Settings {
	public:		
		//! Sets whether the exception handler / error recovery system should be installed
		Settings& installExceptionHandler( bool exceptionHandler = true ) { mInstallExceptionHandler = exceptionHandler; return *this; }
		//! Sets whether Live++ starts minimized, maximized, or normal.
		Settings& initialWindowMode( WindowMode mode ) { mInitialWindowMode = mode; return *this; }
		//! Sets if and under what circumstances Live++ should get focus upon re-compiling.
		Settings& receiveFocusOnRecompile( FocusMode mode ) { mReceiveFocusOnRecompile = mode; return *this; }
		//! When continuous compilation is turned on, Live++ waits for change notifications in the given directory (and its sub-directories) and automatically compiles any changes once the timeout has expired.
		Settings& enableContinuousCompilation( bool enabled = true ) { mEnableContinuousCompilation = enabled; return *this; }
		//! Directory to watch 	Lets you set the directory that is being watched for changes when using continuous compilation.
		Settings& continuousCompilationPath( const std::filesystem::path &path ) { mContinuousCompilationPath = path; return *this; }
		//! When listening for change notifications, Live++ will wait for subsequent changes until the timeout is reached.
		Settings& continuousCompilationTimeout( int32_t ms ) { mContinuousCompilationTimeout = ms; return *this; }
		//! Enables restart from the Lpp App
		Settings& enableRestart( bool enabled = true ) { mEnableRestart = enabled; if( enabled ) return enableMultiProcessEditing(); else return *this; }
		//! By default, you can launch several executables and register them at the same process group, but only if no patches have been applied yet. When this setting is enabled, registered modules will automatically load all available patches when registering with a process group. 
		Settings& enableMultiProcessEditing( bool enabled = true ) { mEnableMultiProcessEditing = enabled; return *this; }
		// Specifies additional options passed to the compiler when creating a patch
		Settings& additionalCompilerOptions( const std::wstring &options ) { mAdditionalCompilerOptions = options; return *this; }
		Settings();
	protected:

		void apply( void* lppModule ) const;
		bool mInstallExceptionHandler;
		bool mEnableRestart;
		std::optional<bool> mEnableMultiProcessEditing;
		std::optional<WindowMode> mInitialWindowMode;
		std::optional<FocusMode> mReceiveFocusOnRecompile;
		std::optional<bool> mEnableContinuousCompilation;
		std::filesystem::path mContinuousCompilationPath;
		int32_t mContinuousCompilationTimeout;
		std::wstring mAdditionalCompilerOptions;
		friend LivePlusPlus;
	};

	//! Returns the global instance
	static LivePlusPlus& instance();
	//! Initialize the global instance
	static void initialize( const Settings &settings = Settings(), const std::filesystem::path &lppPath = LPP_PATH );
	//! Specifies when the the syncing point should happen
	static void syncPoint();
	
	//! Used to keep track of the lifetime of callbacks
	class Connection {
	public:
		Connection() = default;
		Connection( uint32_t id, class LivePlusPlus* parent );
		Connection( const Connection &other ) = delete;
		Connection& operator=( const Connection &other ) = delete;
		//! Removes this callback from Lpp
		void disconnect();
		//! Returns the unique id associated with this callback
		uint32_t getId() const;
		//! Returns the FileWatcher that issued this Connection
		class LivePlusPlus* getParent() const;
	protected:
		uint32_t			mId;
		class LivePlusPlus*	mParent;
		friend class LivePlusPlus;
	};

	//! Raii version of Connection 
	class ScopedConnection : public Connection {
	public:
		ScopedConnection() : Connection() {}
		ScopedConnection( const Connection &other );
		ScopedConnection& operator=( const Connection &other );
		~ScopedConnection();
	};
	
	//! Adds a function to be called before a live++ patch is applied
	Connection addPrePatchFn( const std::function<void()> &fn );
	//! Adds a function to be called after a live++ patch is applied
	Connection addPostPatchFn( const std::function<void()> &fn );
	//! Removes any target matching the Connection
	void removeFn( uint32_t connectionId );
	//! Removes any target matching the Connection
	void removePrePatchFn( const Connection &connection );
	//! Removes any target matching the Connection
	void removePostPatchFn( const Connection &connection );
	
	LivePlusPlus();
	LivePlusPlus( const Settings &settings, const std::filesystem::path &lppPath = LPP_PATH );
	
	void emitPostPatch();
	void emitPrePatch();
private:
	void initializeLpp( const Settings &settings, const std::filesystem::path &lppPath );
	void* mModule;
	using Callback = std::pair<uint32_t,std::function<void()>>;
	std::vector<Callback> mPostPatchFunctions;
	std::vector<Callback> mPrePatchFunctions;
	uint32_t mFunctionsIds;
};

#if ! defined( LIVEPLUSPLUS_DISABLE_ALIAS )
	using Lpp = LivePlusPlus;
#endif