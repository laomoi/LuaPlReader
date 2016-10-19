-- require("functions")


local plreader = {}
-- local jsonStr = '{"a#" : "b","g":{"g1": "gv", "g2":2, "g33":[1,"ggg",3]}}'

local lpeg = require("lpeg")

local S = lpeg.S 
local P = lpeg.P 
local R =lpeg.R 
local V = lpeg.V
local C = lpeg.C
local Cf = lpeg.Cf
local Ct = lpeg.Ct
local Cg = lpeg.Cg
local Cc = lpeg.Cc
local Cs = lpeg.Cs

local eof = P(-1)

local newline = P"\n" + "\r\n"

local head_comment = P("//") * (1 - newline) ^0 

local inline_comment = P("/*")*(1-P("*/"))^0*("*/")
local blank = S" \t" + newline 



local str = V("str")
local value = V("value")
local array = V("array")
local pair = V("pair")
local hash = V("hash")

local wide_blank = (blank + inline_comment)^0
local quote_string = P('"') *Cs( (   P("\\\"")/'"'   + (1-P('"'))  )^0)  *P('"')  -- "...",  " ...\"...."
local pure_string = C( (1-S(",;{}()")-blank)^1 )
local  str = quote_string + pure_string

-- local grammar = require('pegdebug').trace({


local grammar = {
    "Start",
    Start = head_comment * blank* hash,

    
    hash = "{" * wide_blank  * Cf( Ct("") * (wide_blank * pair * ";" *wide_blank) ^ 0,rawset) * "}" * wide_blank,
    pair = Cg(str *wide_blank* "=" *wide_blank * value),
    value = (str + hash + array )*wide_blank,
    array = "(" * wide_blank * Ct( (wide_blank * value *","* wide_blank) ^ 0) * ")" * wide_blank,
}
local patt = P(grammar)





plreader.readPl = function(path)
	local file = io.open(path, "r")
    if file then
        local content = file:read("*a")
        io.close(file)
		local result = patt:match(content)
		return result

    end
    return nil

	
end


local result = plreader.readPl(plfile ) 
if result then
	print( cjson.encode(result) )
end


