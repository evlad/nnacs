fname_x="sin1_x.dat";
fname_y="sin1_y.dat";

fid_x = fopen (fname_x, "w");
fid_y = fopen (fname_y, "w");

for x = 0:0.1:4
  fprintf (fid_x, "%g\n", x);

  y = sin(x);
  fprintf (fid_y, "%g\n", y);
end

fclose (fid_x);
fclose (fid_y);

% End of file
