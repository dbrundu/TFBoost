set object 1 rect from 0,0 to 1,-100 fc lt 2 front
set object 2 rect from 39,0 to 40,-100 fc lt 7 front
set xrange [-5:45]
set yrange [-105:5]
set terminal qt size 400,800
set xl 'X'
set yl 'Z' rotate by 0
set term qt font 'Times,13'
set title '3D DETECTOR RANDOM DEPOSIT'
plot for[i=1:1000:10] 'Summed/TCODE'.i.'.txt' using ($6):($5):($8-$6):(-$7) with vectors lw 1 notitle
pause -1
