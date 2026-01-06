function cos(r)
	return math.cos(math.deg(r))
end

function sin(r)
	return math.sin(math.deg(r))
end

function Spiral(py,spd,plusx,type)
	angle = 0
	for n=0,14 do
		for j=1,3 do
			shotA1(683,384,0.06*(3.5-n/18+j/2),angle,1,12)

		end
		wait(33);
		angle = angle + 20
	end
end


function attack()
	r = 0
	angle = 0
	for n=0,48 do
		for j=0,2 do
			shotA1(683+r*cos(angle),384+r*sin(angle),0.06*(3.5-n/18+j/2),angle+180+n+18,1,12)
		end
		r=r+5
		angle = angle-17
		wait(50)
	end

	Spiral(380,300.25,0,1);
	wait(420);
	Spiral(380,3.25,0,-1);
	wait(420);
	Spiral(380,3,-12,1);
	wait(420);
	Spiral(380,3,12,-1);
	wait(420);
	Spiral(480,2.75,-24,1);

	wait(470);

end


function attack2()
	r = 0
	angle = 0
	for n=0,48 do
		for j=0,2 do
			shotA1(683+r*cos(angle)*1.1,384+r*sin(angle),0.06*(3.5-n/18+j/2),angle+180+n+18,1,12)
		end
		r=r+5
		angle = angle-17
		wait(50)
	end

	for n=0,48 do
		for j=0,2 do
			shotA1(683+r*cos(angle)*1.1,384+r*sin(angle),0.06*(3.5-n/18+j/2),-angle+180+n+18,1,12)
		end
		r=r+5
		angle = angle+17
		wait(50)
	end

	attack2()
end