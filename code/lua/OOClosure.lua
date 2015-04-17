mariner = {}

function mariner.new ()
   local self = {}

   local maxhp = 200
   local hp = maxhp

   function self.heal (deltahp)
      hp = math.min (maxhp, hp + deltahp)
   end
   function self.sethp (newhp)
      hp = math.min (maxhp, newhp)
   end
   function self.gethp ()
      return hp
   end

   return self
end

local m1 = mariner.new ()
local m2 = mariner.new ()
m1.sethp (100)
m1.heal (13)
m2.sethp (90)
m2.heal (5)
print ("Mariner 1 has got "..m1.gethp ().." hit points")
print ("Mariner 2 has got "..m2.gethp ().." hit points")

