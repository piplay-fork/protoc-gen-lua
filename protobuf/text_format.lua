--
--------------------------------------------------------------------------------
--  FILE:  text_format.lua
--  DESCRIPTION:  protoc-gen-lua
--      Google's Protocol Buffers project, ported to lua.
--      https://code.google.com/p/protoc-gen-lua/
--
--      Copyright (c) 2010 , 林卓毅 (Zhuoyi Lin) netsnail@gmail.com
--      All rights reserved.
--
--      Use, modification and distribution are subject to the "New BSD License"
--      as listed at <url: http://www.opensource.org/licenses/bsd-license.php >.
--  COMPANY:  NetEase
--  CREATED:  2010年08月05日 15时14分13秒 CST
--------------------------------------------------------------------------------
--

local string = string
local math = math
local print = print
local getmetatable = getmetatable
local table = table
local ipairs = ipairs
local require = require
local type = type
local A = Array

module(...)

local descriptor = require(_PACKAGE.."descriptor")

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

local FieldDescriptor = descriptor.FieldDescriptor

function msg_format_indent(write, msg, indent)
  for field, value in msg:ListFields() do
    local print_field = function(field_value)
      local name = field.name
      write(string.rep(" ", indent))
      if field.type == FieldDescriptor.TYPE_MESSAGE then
        local extensions = getmetatable(msg)._extensions_by_name
        if extensions[field.full_name] then
          write("[" .. name .. "] {\n")
        else
          write(name .. " {\n")
        end
        msg_format_indent(write, field_value, indent + 4)
        write(string.rep(" ", indent))
        write("}\n")
      else
        write(string.format("%s: %s\n", name, field_value))
      end
    end
    if field.label == FieldDescriptor.LABEL_REPEATED then
      for _, k in ipairs(value) do
        print_field(k)
      end
    else
      print_field(value)
    end
  end
end

function msg_format(msg)
  local out = {}
  local write = function(value)
    out[#out + 1] = value
  end
  msg_format_indent(write, msg, 0)
  return table.concat(out)
end

function msg_format_json(msg)
  if type(msg) ~= 'table' then return msg end
  local out = {}
  for field, value in msg:ListFields() do
    local get = function (field_value)
      if field.type == FieldDescriptor.TYPE_MESSAGE then
        return msg_format_json(field_value)
      else
        return field_value
      end
    end
    if field.label == FieldDescriptor.LABEL_REPEATED then
      local arr = A and A{} or {}
      for _, k in ipairs(value) do
        arr:push(get(k))
      end
      out[field.name] = arr
    else
      out[field.name] = get(value)
    end
  end
  return out
end
