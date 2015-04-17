String = {} -- to hold String class methods
mt = {}

function String:new(s)
    return setmetatable({ value = s or '' }, mt)
end

function String:print()
    print(self.value)
end

mt.__add = function (a, b) return String:new(a.value .. b.value) end
mt.__mul = function (a, b) return String:new(string.rep(a.value, b)) end
mt.__index = String -- redicrect queries to the String table

s = String:new('hello ')
s:print()
print(String.new)
