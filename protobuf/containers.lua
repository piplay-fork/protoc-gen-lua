--
--------------------------------------------------------------------------------
--         FILE:  containers.lua
--        USAGE:  ./containers.lua 
--  DESCRIPTION:  
--      OPTIONS:  ---
-- REQUIREMENTS:  ---
--         BUGS:  ---
--        NOTES:  ---
--       AUTHOR:  Sean Lin (NetSnail), <netsnail@gmail.com>
--      COMPANY:  NetEase
--      VERSION:  1.0
--      CREATED:  2010年08月02日 16时15分42秒 CST
--     REVISION:  ---
--------------------------------------------------------------------------------
--
local setmetatable = setmetatable
local table = table
module "containers"

local _RCFC_meta = {
    add = function(self)
        local value = self._message_descriptor._concrete_class()
        local listener = self._listener
        rawset(self, #self + 1, value)
        value:_SetListener(listener)
        if listener.dirty == false then
            listener:Modified()
        end
        return value
    end,
    remove = function(self, key)
        local listener = self._listener
        table.remove(self, key)
        listener:Modified()
    end,
    __newindex = function(self, key, value)
        error("RepeatedCompositeFieldContainer Can't set value directly")
    end
}
_RCFC_meta.__index = _RCFC_meta

function RepeatedCompositeFieldContainer(listener, message_descriptor)
    local o = {
        _listener = listener,
        _message_descriptor = message_descriptor
    }
    return setmetatable(o, _RCFC_meta)
end

local _RSFC_meta = {
    append = function(self, value)
        self._type_checker(value)
        rawset(self, #self + 1, value)
        self._listener:Modified()
    end,
    remove = function(self, key)
        table.remove(self, key)
        self._listener:Modified()
    end,
    __newindex = function(self, key, value)
        error("RepeatedCompositeFieldContainer Can't set value directly")
    end
}
_RSFC_meta.__index = _RSFC_meta

function RepeatedScalarFieldContainer(listener, type_checker)
    local o = {}
    o._listener = listener
    o._type_checker = type_checker
    return setmetatable(o, _RSFC_meta)
end





