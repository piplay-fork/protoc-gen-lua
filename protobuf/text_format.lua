--
--------------------------------------------------------------------------------
--         FILE:  text_format.lua
--        USAGE:  ./text_format.lua 
--  DESCRIPTION:  
--      OPTIONS:  ---
-- REQUIREMENTS:  ---
--         BUGS:  ---
--        NOTES:  ---
--       AUTHOR:  Sean Lin (NetSnail), <netsnail@gmail.com>
--      COMPANY:  NetEase
--      VERSION:  1.0
--      CREATED:  2010年08月05日 15时14分13秒 CST
--     REVISION:  ---
--------------------------------------------------------------------------------
--
local string = string
local math = math
local print = print

module "text_format"

function format(buffer)
    local len = string.len( buffer )	
    for i = 1, len, 16 do		
        local text = ""	
        for j = i, math.min( i + 16 - 1, len ) do	
            text = string.format( "%s  %02x", text, string.byte( buffer, j ) )			
        end			
        print( text )	
    end
end
