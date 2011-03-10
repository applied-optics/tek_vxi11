%out=rescale(in,sc_min,sc_max);
%
%Very cheesy rescaling function, by Steve D. Sharples (August 2006).
%sc_min is your minimum desired value, sc_max is your maximum desired value.
%Works only upto 3D arrays.
function out=rescale(in,sc_min,sc_max);
mn=min(min(min(in)));mx=max(max(max(in)));
tmp=in-mn; % data now goes from 0 to (mx-mn)
tmp2=tmp./(mx-mn); % data now goes from 0 to 1;
tmp3=tmp2.*(sc_max-sc_min); % now goes from 0 to (sc_max-sc_min)
out=tmp3-sc_min; % now goes from sc_min to sc_max
