Camera = {}

KEY_SPACE = 32
KEY_A = 97
KEY_D = 100
KEY_S = 115
KEY_W = 119

function Camera:Init()
	rhombus.Log("Lua Init")
end

function Camera:Update(dt)
	rhombus.Log("Lua Update")

	speed = 8.0
	velX = 0
	velY = 0

	if rhombus.IsKeyDown(KEY_A) then
		returnString = "A"
		velX = -1
	elseif rhombus.IsKeyDown(KEY_D) then
		returnString = "D"
		velX = 1
	end
	
	if rhombus.IsKeyDown(KEY_W) then
		returnString = "W"
		velY = 1
	elseif rhombus.IsKeyDown(KEY_S) then
		returnString = "S"
		velY = -1
	end

	velX = velX * speed
	velY = velY * speed

	rhombus.Translate(self, velX * dt, velY * dt)
end