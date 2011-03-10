%% gaussian(w,N,off,norm),norm==0 peak=1,norm=1 area=1;

function g=gaussian(w,N,off,norm)
if(nargin==2)
	off=0;norm=0;
	end;
if(nargin==3)
	norm=0;
	end;

w=w*w;
g=ones(round(N),1);
h=N/2;
x=-h:(h-1);
x=x+off;
g=exp(-x.*x./w);
if(norm==1)
	g=g./sum(g);
	end;	

