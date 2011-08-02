-- version demo d'une fonction qui remplacera peut être un jour inputnum

-- demande de très bonnes connaissance du lua



local curs=g.curseur
local tonum = lib.tonumber
local tostr = lib.tostring
local wait=g.wait

local function wt(t)
 repeat wait(1) until not key(t)
 end


local function testkey()

while true do
if key(41)	    then minus = not(minus); tch =( minus and ((alpha and petit) or alternatif) ) or ((alpha and gros) or normal) ; wt(41)
elseif key(33) then alpha = not(alpha); tch =( minus and ((alpha and petit) or alternatif) ) or ((alpha and gros) or normal) ; wt(33)
else
	for i = 1,50,1 do
				if key(i) then return i; end
				end
end
end
end
function scanf(Nmin,Nmax,x,y,Numberdep)
local numberdep =Numberdep or ( ( (Nmin>0) and Nmin) or 0 ) -- numberdep =Numberdep si existe sinon min (0, Nmin)
local strndep = tostr(numberdep)
minus = false
alpha = false

normal = {"0",".","e",nil,nil,"1","2","3",nil,nil,"4","5","6",nil,nil,"7","8","9" }
gros = nil
petit = nil
alternatif = nil
tch = normal

local touche = 0
local number = numberdep
local strn,newstr
newstr = strndep

repeat
strn = newstr
repeat
touche = testkey()
if tch[touche]  then
	if strn == "0" then strn = tch[touche]
		else strn = strn .. tch[touche]
		end
elseif touche == 4 then
					if string.sub(strn,-1)=="e" then strn=strn.."-"
					elseif string.sub(strn,-1)=="-" then strn=string.sub(strn,1,#strn-1)
					elseif string.sub(strn,1,1) =="-" then strn = string.sub(strn,2)
						else strn = "-"..strn	end
elseif touche == 19 then if #strn == 1 then strn = "0"
						else strn=string.sub(strn,1,#strn-1) end
end

curs(x,y)
g.print(strn.."  ")
wt(touche)
until touche == 5
number=tonum(strn)
if not(number) then number = 0 end --conversion failed
newstr=tostr(number)
curs(x,y)
g.print(newstr.."    ")
wt(5)

until newstr == strn
return number
end

clear 0
g.misc("prec",9) -- 9 chiffres+lettres dans la conversion du nombre avec tostr

scanf(0,100,1,1,0)
