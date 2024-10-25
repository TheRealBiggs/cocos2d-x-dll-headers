/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#ifndef __SCRIPT_SUPPORT_H__
#define __SCRIPT_SUPPORT_H__

#include "base/ccConfig.h"
#include "platform/CCCommon.h"
#include "base/CCTouch.h"
#include "base/CCEventTouch.h"
#include "base/CCEventKeyboard.h"
#include <map>
#include <string>
#include <list>

/**
 * @addtogroup base
 * @{
 */

#if CC_ENABLE_SCRIPT_BINDING

typedef struct lua_State lua_State;

NS_CC_BEGIN

class TimerScriptHandler;
class Layer;
class MenuItem;
class CallFunc;
class Acceleration;
class Action;

enum ccScriptType {
    kScriptTypeNone = 0,
    kScriptTypeLua,
    kScriptTypeJavascript
};

/**
 * @endcond
 * @js NA
 */

/** ScriptEventType enum*/
enum ScriptEventType
{
    kNodeEvent = 0,
    kMenuClickedEvent,
    kCallFuncEvent,
    kScheduleEvent,
    kTouchEvent,
    kTouchesEvent,
    kKeypadEvent,
    kAccelerometerEvent,
    kControlEvent,
    kCommonEvent,
    kComponentEvent,
    kRestartGame,
    kScriptActionEvent
};

/**
 * The ScriptEvent wrapper the different script data corresponding to the ScriptEventType in to the unified struct.
 * when the corresponding event is triggered, we could call the `sendEvent` of ScriptEngineProtocol to handle the event.
 * @js NA
 */
struct ScriptEvent
{
    /** 
     * The specific type of ScriptEventType.
     *
     * @lua NA
     * @js NA
     */
    ScriptEventType type;
    /** 
     * Pointer point to the different data.
     *
     * @lua NA
     * @js NA
     */
    void* data;
    
    /**
     * Constructor of ScriptEvent.
     *
     * @lua NA
     * @js NA
     */
    ScriptEvent(ScriptEventType inType,void* inData)
    : type(inType),
      data(inData)
    {
    }
};

/** 
 * Don't make ScriptEngineProtocol inherits from Object since setScriptEngine is invoked only once in AppDelegate.cpp,
 * It will affect the lifecycle of ScriptEngine instance, the autorelease pool will be destroyed before destructing ScriptEngine.
 * So a crash will appear on Win32 if you click the close button.
 * @js NA
 */
class CC_DLL ScriptEngineProtocol
{
public:
    
    /** 
     * Constructor of ScriptEngineProtocol.
     *
     * @lua NA
     * @js NA
     */
    ScriptEngineProtocol()
    {}
    
    /**
     * Destructor of ScriptEngineProtocol.
     *
     * @lua NA
     * @js NA
     */
    virtual ~ScriptEngineProtocol() {}
    
    /** 
     * Get the specific script type.
     *
     * @return the specific script type.
     *
     * @lua NA
     * @js NA
     */
    virtual ccScriptType getScriptType() { return kScriptTypeNone; }
    
    /**
     * Reflect the retain relationship to script scope
     */
    virtual void retainScriptObject(Ref* /*owner*/, Ref* /*target*/) {}
    
    /**
     * Add the script object to root object
     */
    virtual void rootScriptObject(Ref* /*target*/) {}
    
    /**
     * Reflect the release relationship to script scope
     */
    virtual void releaseScriptObject(Ref* /*owner*/, Ref* /*target*/) {}
    
    /**
     * Remove the script object from root object
     */
    virtual void unrootScriptObject(Ref* /*target*/) {}
    
    /**
     * Release all children native refs for the given node in script scope
     */
    virtual void releaseAllChildrenRecursive(Node* /*node*/) {}
    
    /**
     * Release all native refs for the given owner in script scope
     */
    virtual void releaseAllNativeRefs(cocos2d::Ref* /*owner*/) {}

    /** 
     * Remove script object,The specific meaning should refer to the ScriptType.
     * For Lua, @see removeScriptObjectByObject of LuaEngine.
     *
     * @lua NA
     * @js NA
     */
    virtual void removeScriptObjectByObject(Ref* /*obj*/) {}
    
    /** 
     * Remove script function handler, only LuaEngine class need to implement this function.
     * @see removeScriptHandler of LuaEngine.
     * @lua NA
     * @js NA
     */
    virtual void removeScriptHandler(int /*handler*/) {}
    
    /** 
     * Reallocate script function handler, only LuaEngine class need to implement this function.
     * @see reallocateScriptHandler of LuaEngine.
     * @lua NA
     * @js NA
     */
    virtual int reallocateScriptHandler(int /*handler*/) { return 0; }
    
    /**
     * Execute script code contained in the given string.
     *
     * @param codes holding the valid script code that should be executed.
     * @return 0 if the string is executed correctly.
     * @return other if the string is executed wrongly.
     * @lua NA
     * @js NA
     */
    virtual int executeString(const char* codes) = 0;
    
    /**
     * Execute a script file.
     * 
     * @param filename String object holding the filename of the script file that is to be executed.
     * @return 0 if it happen the error or it hasn't return value, otherwise it return the value by calling the lua function.
     * @lua NA
     * @js NA
     */
    virtual int executeScriptFile(const char* filename) = 0;
    
    /**
     * Execute a scripted global function.
     * The function should not take any parameters and should return an integer.
     *
     * @param functionName String object holding the name of the function, in the global script environment, that is to be executed.
     * @return The integer value returned from the script function.
     * @lua NA
     * @js NA
     */
    virtual int executeGlobalFunction(const char* functionName) = 0;
    
    /**
     * When trigger a script event ,call this func,add params needed into ScriptEvent object.nativeObject is object triggering the event, can be nullptr in Lua.
     *
     *
     * @lua NA
     * @js NA
     */
    virtual int sendEvent(ScriptEvent* evt) = 0;
    
    /** 
     * Handle the assert message.
     *
     * @return true if the assert was handled by the script engine, false otherwise.
     *
     * @lua NA
     * @js NA
     */
    virtual bool handleAssert(const char *msg) = 0;
    
    /** 
     * Useless for Lua.
     *
     * @lua NA
     * @js NA
     */
    virtual void setCalledFromScript(bool /*callFromScript*/) {}
    
    /** 
     * Useless for Lua.
     *
     * @lua NA
     * @js NA
     */
    virtual bool isCalledFromScript() { return false; };
    
    /** ConfigType enum. */
    enum class ConfigType
    {
        NONE,
        COCOSTUDIO
    };
    /** 
     * Parse configuration file.
     * 
     * @param type the specific type value.
     * @param str the information data.
     *
     * @lua NA
     * @js NA
     */
    virtual bool parseConfig(ConfigType type, const std::string& str) = 0;


    /** Root a Reference.
     It tells the Garbage Collector that the associated Scripting object should not be collected
     */
    virtual void rootObject(Ref* /*obj*/) {}

    /** Unroot a Reference.
     It tells the Garbage Collector that the associated Scripting object can be collected
     */
    virtual void unrootObject(Ref* /*obj*/) {}

    /** Triggers the garbage collector */
    virtual void garbageCollect() {}
};

class Node;
/**
 * ScriptEngineManager is a singleton which manager an object instance of ScriptEngineProtocl, such as LuaEngine.
 *
 * @since v0.99.5-x-0.8.5
 * @js NA
 */
class CC_DLL ScriptEngineManager
{
public:
    /** 
     * Constructor of ScriptEngineManager.
     *
     * @lua NA
     * @js NA
     */
    ~ScriptEngineManager(void);
    /** 
     * Get the ScriptEngineProtocol object.
     *
     * @return the ScriptEngineProtocol object.
     *
     * @lua NA
     * @js NA
     */
    ScriptEngineProtocol* getScriptEngine(void) {
        return _scriptEngine;
    }
    /**
     * Set the ScriptEngineProtocol object should be managed.
     *
     * @param scriptEngine should be managed.
     *
     * @lua NA
     * @js NA
     */
    void setScriptEngine(ScriptEngineProtocol *scriptEngine);
    
    /**
     * Remove the ScriptEngineProtocol object managed.
     *
     *
     * @lua NA
     * @js NA
     */
    void removeScriptEngine(void);
    /**
     * Get the instance of ScriptEngineManager object.
     *
     * @return the instance of ScriptEngineManager object.
     *
     * @lua NA
     * @js NA
     */
    static ScriptEngineManager* getInstance();
    /**
     * Destroy the singleton about ScriptEngineManager.
     *
     * @lua NA
     * @js NA
     */
    static void destroyInstance();
    /**
     *
     *
     * @lua NA
     * @js NA
     */
    static bool sendActionEventToJS(Action* actionObject, int eventType, void* param);
    /**
     *
     *
     * @lua NA
     * @js NA
     */
    static bool sendNodeEventToJS(Node* node, int action);
    /**
     *
     *
     * @lua NA
     * @js NA
     */
    static bool sendNodeEventToJSExtended(Node* node, int action);
    /**
     * Call the Lua function when the event of node is triggered.
     * 
     * @param node the nativeobject triggers the event.
     * @param action the specific type.
     *
     * @lua NA
     * @js NA
     */
    static void sendNodeEventToLua(Node* node, int action);
    /**
     * @deprecated Use getInstance() instead.
     *
     * @lua NA
     * @js NA
     */
    CC_DEPRECATED_ATTRIBUTE static ScriptEngineManager* sharedManager() { return ScriptEngineManager::getInstance(); };
    /**
     * @deprecated Use destroyInstance() instead.
     *
     * @lua NA
     * @js NA
     */
    CC_DEPRECATED_ATTRIBUTE static void purgeSharedManager() { ScriptEngineManager::destroyInstance(); };
    
private:
    ScriptEngineManager(void)
    : _scriptEngine(nullptr)
    {
    }
    
    ScriptEngineProtocol *_scriptEngine;
};

NS_CC_END



#endif // #if CC_ENABLE_SCRIPT_BINDING

// end group
/// @}

#endif // __SCRIPT_SUPPORT_H__
