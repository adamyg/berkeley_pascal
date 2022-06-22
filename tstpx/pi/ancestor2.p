program ancestor2(output);
{ancestor algorithm using sets instead of boolean matrix}
  const n = 63;
  var i: integer;
      r: array [1..n] of set of 1..n;
      j: integer;
begin
  i:= 1;
  if i = 1 then r[i] := [i+1];
  j:= 2;
  if j in r[i] then write('@');
end .
