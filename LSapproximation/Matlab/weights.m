%% compute weights based on Euclidean distances
%
%

N = 20;
file = fopen( 'weights.txt', 'wt');
fprintf( file, '{');
for n = 0:N
    fprintf( file, '{');
    for m= 0:N
        d = sqrt( n*n + m*m);
        w = sqrt( 0.9^d);
        fprintf( file, '%.3f, ', w);
    end
    fprintf( file, '},\n');
end
fprintf( file, '};\n');
fclose( file);