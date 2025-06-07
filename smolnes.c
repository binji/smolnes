#include <SDL2/SDL.h>                                                                 /************************************************************/
#include <stdint.h>                                                                   /*    SMOLNES by Ben Smith (Oct 2022) (updated Jun 2025)    */
#define k if                                                                          /* -------------------------------------------------------- */
#define _ 1024                                                                        /*                                                          */
#define G goto                                                                        /* A NES emulator that can play mapper 0/1/2/3/4/7 games.   */
#define Q return                                                                      /* Requires gcc/clang and Linux/macOS.                      */
#define Z switch                                                                      /*                                                          */
#define z m(++St,1)                                                                   /* $ cc smolnes.c -O2 -lSDL2 -o smolnes                     */
#define W break;case                                                                  /* $ smolnes <rom.nes>                                      */
#define BD break;default:                                                             /*                                                          */
#define m(l,h)mm(l,h,0,0)                                                             /* Controls: DPAD=Arrow keys B=Z A=X Start=Enter Select=Tab */
#define g(x)mm(St--,1,x,1);                                                           /*                                                          */
#define x(a,b)case a:case b:                                                          /************************************************************/
typedef uint8_t u;typedef uint16_t U;u*E,*C,K[4],M[8],e=14,Mb=12,S[8],MB,MI,ML,A,X,Y,P=4,St=~2,h,l,i,j,L,r,v,t,pm,p,H,pb,pw,fx,o,N,nb,pl,vr[_*2],Pr[
_],D,ra[_*8],cr[_*8],pr[_*8],oa[_],bm[]="\200@\1\2\1\0\0\1\4\0\0\4\0\0@\0\b\0\0\b",ks,mb,F,I,c0,c1,J,b[_*_],*kb;U y,T,V,f,d,at,a,s,c,fb[_*_],yy=992,
xx=35871;u*q(U a){Q&C[M[a>>Mb]<<Mb|a%(1<<Mb)];}u*nt(U a){Q&vr[!D?a%_:D^1?D^2?a/2&_|a%_:a&2047:a%_+_];}u mm(u l,u h,u v,u w){U a=h<<8|l;Z(h>>=4){x(0,
1)Q+w?ra[a]=v:ra[a];x(2,3)k((l&=7)==7){t=pb;u*E=V<_*8?w&C!=cr?&t:q(V):V<16128?nt(V):Pr+(u)(V&19^16?V:V^16);w?*E=v:(pb=*E);V+=p&4?32:1;V%=16*_;Q+t;}k
(w)Z(l){W+0:T=T&62463|v%4*_;p=v;W+1:pm=v;W+5:T=(pw^=1)?fx=v&7,T&~31|v/8:T&xx|v%8<<12|v*4&yy;W+6:T=!(pw^=1)?V=T&~255|v:(u)T|v%64<<8;}k(l==2)Q t=H&224
,H&=127,                                                                                                                                     pw=0,t;
W+4:k(20  ==l&&w)for(U f=256;f--;)oa[f]=m(f,v);for(t=0,h=8;h--;)t=t*2+kb["\33\35+(RQPO"[h]];Q+l==22?w?ks=t:(t=ks&1,ks/=2,t):0;x(6,7)a&=8191  ;Q w?pr
[a]=v:pr  [a];BD k(w)Z(b[6]>>4){W+7:D=!(v/16);K[1]=(*K=v%8*2)+1;W+4:w=a&1;Z(h>>1){W+4:*(w?&S[MB&7]:&MB)=v;t=MB>>5&4;for(l=4;l--;)M[l+t]=S[l  /2]&~!(
l%2)|l%2  ,M[4+l-t]=S[2+l];K[t=MB/32&2]=S[6];K[1]=S[7];K[2-t]=(K[3]=b[4]*2-1)-1;W+5:k(!w)D=2+v%2;W+6:k(!w)ML=v;W+7:MI=w;}W+3:M[1]=(*M=v%4*2  )+1;W+2
:*K=v&31  ;W+1:v&128?F=0,mb=5,I|=12:(F=F/2|v<<4&16,                                       !--mb)?t=a>>13,*(t==4?D=F&3,&I:t^5?t^6?&J:&c1:&c0  )=F,mb=
5,*M=c0&  ~!(I&16),M[1]=I&16?c1:c0|1,t=I/4%4-2,*K=!                                       t?0:t^1?J&~1:J,K[1]=!t?J:t^1?J|1:b[4]-1:0;}Q+E[(K  [h-8>>e
-12]&(b[  4]<<14-e)-1)<<e|a%(1<<e)];}Q~0;}u R(){v=m(l,h);!++l&&++h;Q+v;}u n(u v){Q P=P&125|v&128|!v*2;}int main(int w,char**av){SDL_RWread(  SDL_RW\
FromFile  (av[1],"rb"),b,_*_,1);SDL_Init(32);void*re=SDL_CreateRenderer(SDL_CreateWindow("smolnes",0,0,_,840,4),-1,4),*tx=SDL_CreateTexture  (re,35\
7896194,  1,256,224);             E=b+16;K[1]=(t=b[                                       4])-1;C=b[5]?E+(t<<14):cr;M[1]=(b[5]||1)*2-1;D=3-  b[6]%2;
k(4==b[6  ]/16)mm(0,+  128,0,1),  Mb-=2,e--;l=m(~3,                                       ~0);h=m(~2,~0);w=0;kb=SDL_GetKeyboardState(0);Y:c  =L=0;k(
N)G N;Z(  f=31&(o=R()  )){W+0:k(  o&128){R();L=1;G n;}Z(o>>5){W+0:!++l&&++h;N:g(h)g(l)g(P|32)l=m(N=~1-(N&4),~0);h=m(N+1,~0);N=0;c++;W+1:r=R  ();g(h)
g(l)h=R(  );l=r;W+2:P  =z&~32;l=  z;h=z;W+3:l=z;h=z;!++l&&++h;}c+=4;W+16:R();k(!(P&bm[o>>6])^o/32&1)r=l+(int8_t)v>>8,h+=r,l+=v,c+=r?2:1;W-1  :x(8,24
)Z(o>>=4  ){W          0:g(P|48)          c++;W+2:P                                       =z&~16;c+=2;W+4:g(A)c++;W+6:n(A=z);c+=2;W+8:n(--Y  );W+9:n
(A=Y);W+  10:  n(Y=A);W+12   :n(++Y);;W+  14:n(++X)                                       ;BD P=P&~bm[o+3]|bm[o+4];}W-2:x(10,26)Z(o>>4){W+8  :n(A=X)
;W+9:St=  X;W  10:n(X=A)       ;W+11:n(X  =St);W+12:n(--X);W+14:BD L=1;v=A;G n;}W+1:R();i=m(v+=X                 ,                 0);j=m(v  +1,0);c
+=4;G o;  x(2  ,9)R();L=       1;G n;;W+  17:i=m(R(),0);j=m(v+1,0);c++;G A;x(4,5)x(6,20)x(21,22)  i=R();k(r=f>6  )  i+=150^214&o?  X:Y;j=0;  c-=!r;G
o;W+12:x  (13  ,14)x(25,       28)x(29,+  30)i=R()                                         ;j=R(  );k(     f<25  )  G o;     A:v=  f<28|190  ==o?Y:X
;j+=r=i+  v>+  255;i+=v;c=   c+(128==(o&  224)|o!=  190&o%16==14)|r;o:c+=2;k(o^76&&o&224^  128)v  =m         (i  ,  j)         ;n  :r=0;Z(o  &227){W
1:n(A|=v  );W          33:n(A&=v          );W+65:n  (A^=v        );W+225:v=        ~v;x(-  1,97)  f           =  A  +           v  +P%2;P=P  &~65|f>
255|((A^  f)&128&(v^f  ))/2;n(A=  f);W+34:r=P&1;x(  2,-2          )r|=v*2;          P=P&~  1|v>>  7;         ;G  m  ;W         98  :r=P<<7;  x(66,-3
)r|=v/2;  P=P&~1|v&1;  G m;W+194  :r=v-1;G m;W+226  :r=v+        1;m:n(r);L        ?A=r:(  c+=2,  mm(i     ,j,r  ,  1));     W+32  :P=P&61|  192&v|!
(A&v)*2;  W+64:l=i;h=  j;c--;;W+  96:l=v;h=m(i+1,j  );c++;BD{f=o/32;u*e=o%4^2&&f^7?o%4^1?  &Y:&A  :&X;f^4?f^5?P  =  P&~1|v<=*e,n(  *e-v):n(  *e=v):+
mm(i,j,*  e,1);}}}for             (t=c*3+6;t--;){k                                         (+pm&                 +                 24){k(y<  240){k(
-256+d>+  63u){k(d<256){u C=a>>14-fx&2|s>>15-fx&1,P=w>>28-fx*2&12,T[]="1%(+/0/,)&'%%%%%:&.278730'''(%%%D16;@AA>:30+.-%%D<>@BDCA@><;;:%%=%("  "))%(-\
25630%%"  "%O8.)*,29?CEB>%%%dKD?@AFLQUXUT5%%dZWVUUXX[]^_^P%%16841'%&&&'(/%%%:=@>:.%%''&.7%%%DDDDC:0)%%.6>-%%DDDDDA=988<>B:%%";k(pm&16)for(u  *s=oa;s
<oa+256;                                                                                                                                     s+=4){U
h=p&32?16:8,X=d-s[3],Y=y-*s-1;k(X<8&Y<h){U t=s[1],r=s[2],i=X^!(r&64)*7,j=Y^(r&128?h-1:0),a=(p&32?t%2<<12|t<<4&-32|j*2&16:(p&8)<<9|t<<4)|j&7,c=*q(a+8
)>>i<<1&2|*q(a)>>i&1;k(c){C&&32&r?0:(C=c,P=16|r*4&12);C&&s==oa?H|=64:0;G X;}}}X:C=Pr[C?P|C:0];fb[y*256+d]=T[C+128]-37<<11|T[C+64]-37<<5|T[C]-37;}k(d
<336)a*=2,s*=2,w*=4;f=V>>12|p<<8&_*4|nb<<4;Z(d&7){W+1:nb=*nt(V);W+3:at=(*nt(V&3072|V>>4&56|960|V/4&7)>>(V>>5&2|V/2&1)*2)%4*21845;W+5:pl=*q(f);W+7:V=
V%32^31?V+1:V&~31^_;s|=pl,a|=*q(f|8),w|=at;}}k(d==256)V=(V>>12&7^7?V+4*_:V&yy^928?V&yy^yy?V&xx|V+32&yy:V&xx:V&xx^2*_)&~1055|T&1055;}k((y+1)%262<241&
d==261&MI&&!ML--)N=1;k(y==261&25u>d-280)V=V&33823|T&31712;}k(d==1){k(y==241){k(p&128)N=4;H|=128;SDL_UpdateTexture(tx,0,fb+_*2,512);SDL_RenderCopy(re
,tx,0,0);SDL_RenderPresent(re);for(SDL_Event ev;SDL_PollEvent(&ev);)k(ev.type==256)Q+0;}k(y==261)H=0;}k(++d==341)d=0,y++,y%=262;}G Y;}U _binji_2025;
