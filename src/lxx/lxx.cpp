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
#include "lxx/lxx.h"
#include "LivePP/API/x64/LPP_API_x64_CPP.h"

lxx::lxx()
	: mSynchronizedAgent( nullptr )
{
}

lxx::~lxx()
{
	if( mSynchronizedAgent ) {
		lpp::LppDestroySynchronizedAgent( mSynchronizedAgent.get() );
	}
}

lxx& lxx::instance()
{
	static lxx inst;
	return inst;
}

void lxx::initDefaultAgent( const lpp::LppProjectPreferences* projectPreferences, const std::filesystem::path &lppPath )
{
	lxx& inst = instance();
	inst.mDefaultAgent = std::make_unique<lpp::LppDefaultAgent>( lpp::LppCreateDefaultAgentWithPreferences( nullptr, lppPath.wstring().c_str(), projectPreferences ) );

	if( !lpp::LppIsValidDefaultAgent( inst.mDefaultAgent.get() ) ) {
		throw std::exception( ( "Failed to initialize Live++ Default Agent from " + lppPath.string() ).c_str() );
	}

	inst.mDefaultAgent->EnableModule( lpp::LppGetCurrentModulePath(), lpp::LPP_MODULES_OPTION_ALL_IMPORT_MODULES, nullptr, nullptr );
}

void lxx::initDefaultAgent( const std::filesystem::path &projectPreferencesPath, const std::filesystem::path &lppPath )
{
	lxx& inst = instance();
	inst.mDefaultAgent = std::make_unique<lpp::LppDefaultAgent>( lpp::LppCreateDefaultAgentWithPreferencesFromFileANSI( nullptr, lppPath.string().c_str(), projectPreferencesPath.string().c_str() ) );

	if( !lpp::LppIsValidDefaultAgent( inst.mDefaultAgent.get() ) ) {
		throw std::exception( ( "Failed to initialize Live++ Default Agent from " + lppPath.string() ).c_str() );
	}

	inst.mDefaultAgent->EnableModule( lpp::LppGetCurrentModulePath(), lpp::LPP_MODULES_OPTION_ALL_IMPORT_MODULES, nullptr, nullptr );
}

void lxx::initSynchronizedAgent( const lpp::LppProjectPreferences* projectPreferences, const std::filesystem::path &lppPath )
{
	lxx& inst = instance();
	inst.mSynchronizedAgent = std::make_unique<lpp::LppSynchronizedAgent>( lpp::LppCreateSynchronizedAgentWithPreferences( nullptr, lppPath.wstring().c_str(), projectPreferences ) );

	if( !lpp::LppIsValidSynchronizedAgent( inst.mSynchronizedAgent.get() ) ) {
		throw std::exception( ( "Failed to initialize Live++ Syncrhonized Agent from " + lppPath.string() ).c_str() );
	}

	inst.mSynchronizedAgent->EnableModule( lpp::LppGetCurrentModulePath(), lpp::LPP_MODULES_OPTION_ALL_IMPORT_MODULES, nullptr, nullptr );
}

void lxx::initSynchronizedAgent( const std::filesystem::path &projectPreferencesPath, const std::filesystem::path &lppPath )
{
	lxx& inst = instance();
	inst.mSynchronizedAgent = std::make_unique<lpp::LppSynchronizedAgent>( lpp::LppCreateSynchronizedAgentWithPreferencesFromFileANSI( nullptr, lppPath.string().c_str(), projectPreferencesPath.string().c_str() ) );

	if( !lpp::LppIsValidSynchronizedAgent( inst.mSynchronizedAgent.get() ) ) {
		throw std::exception( ( "Failed to initialize Live++ Syncrhonized Agent from " + lppPath.string() ).c_str() );
	}

	inst.mSynchronizedAgent->EnableModule( lpp::LppGetCurrentModulePath(), lpp::LPP_MODULES_OPTION_ALL_IMPORT_MODULES, nullptr, nullptr );
}

lpp::LppSynchronizedAgent& lxx::synchronizedAgent()
{
	if( ! instance().mSynchronizedAgent ) {
		throw std::exception( "Initialize Live++ before calling lxx::agent" );
	}
	return *instance().mSynchronizedAgent;
}

bool lxx::wantsReload( bool synchronizeWithCompilation )
{
	return synchronizedAgent().WantsReload( synchronizeWithCompilation ? lpp::LPP_RELOAD_OPTION_SYNCHRONIZE_WITH_COMPILATION_AND_RELOAD : lpp::LPP_RELOAD_OPTION_SYNCHRONIZE_WITH_RELOAD );
}

void lxx::reload( bool waitUntilChangesAreApplied )
{
	synchronizedAgent().Reload( waitUntilChangesAreApplied ? lpp::LPP_RELOAD_BEHAVIOUR_WAIT_UNTIL_CHANGES_ARE_APPLIED : lpp::LPP_RELOAD_BEHAVIOUR_CONTINUE_EXECUTION );
}

bool lxx::wantsRestart()
{
	return synchronizedAgent().WantsRestart();
}

void lxx::restart()
{
	synchronizedAgent().Restart( lpp::LPP_RESTART_BEHAVIOUR_INSTANT_TERMINATION, 0u, nullptr );
}