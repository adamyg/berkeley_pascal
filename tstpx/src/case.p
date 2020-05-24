program prog(input,output);

type	sint	= -1000..1000;
	cint	= -128..127;

var	i	:integer;
	j 	:sint;
	c	:cint;

begin
	for i := 500 to 501 do
	begin
		write('(', i:3, ')');
		case i of
		500: write('Case');
		501: writeln(' checks');
		end;
	end;

	for j := 500 to 501 do
	begin
		write('(', j:3, ')');
		case j of
		500: write('Case');
		501: writeln(' checks');
		end;
	end;

	for c := 100 to 101 do
	begin
		write('(', c:3, ')');
		case c of
		100: write('Case');
		101: writeln(' checks');
		end;
	end;
end.
