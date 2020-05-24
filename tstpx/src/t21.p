program  t21(output);

function BOOL(val:integer):boolean;
begin
         BOOL := true;
end;

function INT(val:integer):integer;
begin
         INT := 42;
end;

begin
         if (BOOL(1)) then
             begin
                  writeln( 'true' );
             end
         else 
             begin
                  writeln( 'false' );
             end;

         if (INT(1) = 42) then
             begin
                  writeln( 'true' );
             end
         else 
             begin
                  writeln( 'false' );
             end;
end.

