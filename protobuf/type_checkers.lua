--
--------------------------------------------------------------------------------
--         FILE:  type_checkers.lua
--        USAGE:  ./type_checkers.lua 
--  DESCRIPTION:  
--      OPTIONS:  ---
-- REQUIREMENTS:  ---
--         BUGS:  ---
--        NOTES:  ---
--       AUTHOR:  Sean Lin (NetSnail), <netsnail@gmail.com>
--      COMPANY:  NetEase
--      VERSION:  1.0
--      CREATED:  2010年07月29日 19时30分37秒 CST
--     REVISION:  ---
--------------------------------------------------------------------------------
--

local type = type
local error = error
local string = string

module "type_checkers"
function TypeChecker(acceptable_types)
    local acceptable_types = acceptable_types

    return function(proposed_value)
        local t = type(proposed_value)
        if acceptable_types[type(proposed_value)] == nil then
            error(string.format('%s has type %s, but expected one of: %s', 
                proposed_value, type(proposed_value), acceptable_types))
        end
    end
end

function Int32ValueChecker()
    local _MIN = -2147483648
    local _MAX = 2147483647
    return function(proposed_value)
        if type(proposed_value) ~= 'number' then
            error(string.format('%s has type %s, but expected one of: number',
            proposed_value, type(proposed_value)))
        end
        if _MIN > proposed_value or proposed_value > _MAX then
            error('Value out of range: ' .. proposed_value)
        end
    end
end

function Uint32ValueChecker(IntValueChecker)
    local _MIN = 0
    local _MAX = 0xffffffff

    return function(proposed_value)
        if type(proposed_value) ~= 'number' then
            error(string.format('%s has type %s, but expected one of: number',
                proposed_value, type(proposed_value)))
        end
        if _MIN > proposed_value or proposed_value > _MAX then
            error('Value out of range: ' .. proposed_value)
        end
    end
end

function UnicodeValueChecker()
    return function (proposed_value)
        if type(proposed_value) ~= 'string' then
            error('%s has type %s, but expected one of: string', proposed_value, type(proposed_value))
        end
    end
end
