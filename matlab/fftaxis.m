%[z,z_not_shifted]=fftaxis(t)
function [z,z_not_shifted]=fftaxis(t)

s=max(size(t));

hint=(t(2)-t(1))*s;

%% produces the z-axis... fine up till 1/2 way
z_not_shifted=[0:(s-1)]*(1/hint);

%% the following line produces the real scale, but it doesn't plot
%% correctly
%z_proper=(s/2:s)=z_not_shifted(s)-z(1:(s/2)-1);

z=[(-s/2)+1:s/2]*(1/hint);
%z=[(-s/2):(s/2)-1]*(1/hint);

