--
--------------------------------------------------------------------------------
--         FILE:  listener.lua
--        USAGE:  ./listener.lua 
--  DESCRIPTION:  
--      OPTIONS:  ---
-- REQUIREMENTS:  ---
--         BUGS:  ---
--        NOTES:  ---
--       AUTHOR:  Sean Lin (NetSnail), <netsnail@gmail.com>
--      COMPANY:  NetEase
--      VERSION:  1.0
--      CREATED:  2010年08月02日 17时35分25秒 CST
--     REVISION:  ---
--------------------------------------------------------------------------------
--
local setmetatable = setmetatable

module "listener"

local _null_listener = {
    Modified = function()
    end
}

function NullMessageListener()
    return _null_listener
end

local _listener_meta = {
    Modified = function(self)
        if self.dirty then
            return
        end
        if self._parent_message then
            self._parent_message:_Modified()
        end
    end
}

function Listener(parent_message)
    local o = {}
    o.__mode = "v"
    o._parent_message = parent_message
    o.dirty = false
    return setmetatable(o, _listener_meta)
end

