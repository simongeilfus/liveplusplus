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
#include "liveplusplus/liveplusplus.h"

#if defined(_WIN32)
	#define NOMINMAX
	#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <LPP_API.h>
//#include <LPP_ForceLinkStaticRuntime.h>

LivePlusPlus::LivePlusPlus()
	: mFunctionsIds( 0 ),
	mModule( nullptr )
{
}

LivePlusPlus::LivePlusPlus( const Settings &settings, const std::filesystem::path &lppPath )
	: LivePlusPlus()
{
	initializeLpp( settings, lppPath );
}

LivePlusPlus& LivePlusPlus::instance()
{
	static LivePlusPlus inst;
	return inst;
}

LivePlusPlus::Connection::Connection( uint32_t id, LivePlusPlus* parent )
	: mId( id ), mParent( parent )
{
}

void LivePlusPlus::Connection::disconnect()
{
	if( mParent ) {
		mParent->removeFn( mId );
	}
}

uint32_t LivePlusPlus::Connection::getId() const 
{ 
	return mId; 
}

LivePlusPlus* LivePlusPlus::Connection::getParent() const 
{ 
	return mParent; 
}

LivePlusPlus::ScopedConnection::ScopedConnection( const Connection &other )
{
	mId = other.getId();
	mParent = other.getParent();
}

LivePlusPlus::ScopedConnection& LivePlusPlus::ScopedConnection::operator=( const Connection &other )
{
	mId = other.getId();
	mParent = other.getParent();
	return *this;
}

LivePlusPlus::ScopedConnection::~ScopedConnection() 
{ 
	disconnect(); 
}
	
LivePlusPlus::Connection LivePlusPlus::addPrePatchFn( const std::function<void()> &fn )
{
	uint32_t id = mFunctionsIds++;
	mPrePatchFunctions.push_back( { id, fn } );
	return Connection( id, this );
}

LivePlusPlus::Connection LivePlusPlus::addPostPatchFn( const std::function<void()> &fn )
{
	uint32_t id = mFunctionsIds++;
	mPostPatchFunctions.push_back( { id, fn } );
	return Connection( id, this );
}

void LivePlusPlus::removeFn( uint32_t connectionId )
{
	for( auto it = mPostPatchFunctions.begin(); it != mPostPatchFunctions.end(); ) {
		if( it->first == connectionId ) {
			it = mPostPatchFunctions.erase( it );
		}
		else {
			++it;
		}
	}
	for( auto it = mPrePatchFunctions.begin(); it != mPrePatchFunctions.end(); ) {
		if( it->first == connectionId ) {
			it = mPrePatchFunctions.erase( it );
		}
		else {
			++it;
		}
	}
}

void LivePlusPlus::removePrePatchFn( const Connection &connection )
{
	removeFn( connection.getId() );
}

void LivePlusPlus::removePostPatchFn( const Connection &connection )
{
	removeFn( connection.getId() );
}

void LivePlusPlus::emitPostPatch()
{
	for( const auto &fn : mPostPatchFunctions ) {
		fn.second();
	}
}

void LivePlusPlus::emitPrePatch()
{
	for( const auto &fn : mPrePatchFunctions ) {
		fn.second();
	}
}

void postPatch()
{
	LivePlusPlus::instance().emitPostPatch();
}

LPP_POSTPATCH_HOOK( postPatch );

void prePatch()
{
	LivePlusPlus::instance().emitPrePatch();
}

LPP_PREPATCH_HOOK( prePatch );

void LivePlusPlus::initialize( const LivePlusPlus::Settings &settings, const std::filesystem::path &lppPath )
{
	instance().initializeLpp( settings, lppPath );
}

void LivePlusPlus::initializeLpp( const LivePlusPlus::Settings &settings, const std::filesystem::path &lppPath )
{
	mModule = lpp::lppLoadAndRegister( lppPath.wstring().c_str(), "Live++ProcessGroup" );
	
	if( ! mModule ) {
		throw std::exception( ( "Failed to load Live++ library at " + lppPath.string() ).c_str() );
	}
	lpp::lppEnableAllCallingModulesAsync( static_cast<HMODULE>( mModule ) );

	settings.apply( mModule );
}

void LivePlusPlus::syncPoint()
{
	lpp::lppSyncPoint( static_cast<HMODULE>( instance().mModule ) );
}

LivePlusPlus::Settings::Settings() 
	: mInstallExceptionHandler( false ),
	mInitialWindowMode( WindowMode::Minimized ),
	mReceiveFocusOnRecompile( FocusMode::OnError ),
	mEnableContinuousCompilation( false ),
	mContinuousCompilationTimeout( -1 ),
	mEnableRestart( false ),
	mEnableMultiProcessEditing( false )
{
}

namespace {
	const wchar_t* getWindowModeString( LivePlusPlus::WindowMode mode )
	{
		if( mode == LivePlusPlus::WindowMode::Maximized ) return L"Maximized";
		else if( mode == LivePlusPlus::WindowMode::Minimized ) return L"Minimized";
		else if( mode == LivePlusPlus::WindowMode::Normal ) return L"Normal";
		return L"";
	}

	const wchar_t* getFocusModeString( LivePlusPlus::FocusMode mode )
	{
		if( mode == LivePlusPlus::FocusMode::Never ) return L"Never";
		else if( mode == LivePlusPlus::FocusMode::OnError ) return L"On error";
		else if( mode == LivePlusPlus::FocusMode::OnShortcut ) return L"On shortcut";
		else if( mode == LivePlusPlus::FocusMode::OnSuccess ) return L"On success";
		return L"";
	}
}

void LivePlusPlus::Settings::apply( void* lppModule ) const
{
	HMODULE lppHModule = static_cast<HMODULE>( lppModule );
	if( mInstallExceptionHandler ) {
		lpp::lppInstallExceptionHandler( lppHModule );
	}
	if( mInitialWindowMode ) {
		lpp::lppApplySettingString( lppHModule, "initial_window_mode", getWindowModeString( mInitialWindowMode.value() ) );
	}
	if( mReceiveFocusOnRecompile ) {
		lpp::lppApplySettingString( lppHModule, "receive_focus_on_recompile", getFocusModeString( mReceiveFocusOnRecompile.value() ) );
	}
	if( mEnableContinuousCompilation ) {
		lpp::lppApplySettingBool( lppHModule, "continuous_compilation_enabled", mEnableContinuousCompilation.value() );
	}
	if( !mContinuousCompilationPath.empty() ) {
		lpp::lppApplySettingString( lppHModule, "continuous_compilation_path", mContinuousCompilationPath.wstring().c_str() );
	}
	if( mContinuousCompilationTimeout != -1 ) {
		lpp::lppApplySettingInt( lppHModule, "continuous_compilation_timeout", mContinuousCompilationTimeout );
	}
	if( mEnableMultiProcessEditing ) {
		lpp::lppApplySettingBool( lppHModule, "install_compiled_patches_multi_process", mEnableMultiProcessEditing.value() );
	}
	if( !mAdditionalCompilerOptions.empty() ) {
		lpp::lppApplySettingString( lppHModule, "additional_compiler_options", mAdditionalCompilerOptions.c_str() );
	}
}