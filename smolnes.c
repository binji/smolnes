#include <SDL2/SDL.h>                                                                        /************************************************************/
#include <stdint.h>                                                                          /*    SMOLNES by Ben Smith (Oct 2022) (updated Jan 2024)    */
#define k if                                                                                 /* -------------------------------------------------------- */
#define _ 1024                                                                               /*                                                          */
#define G goto                                                                               /* A NES emulator that can play mapper 0/1/2/3/4/7 games.   */
#define Q return                                                                             /* Requires gcc/clang and Linux/macOS.                      */
#define Z switch                                                                             /*                                                          */
#define W break;case                                                                         /* $ cc smolnes.c -O2 -lSDL2 -o smolnes                     */
#define O(a)x(a,(a)+16,~(a))                                                                 /* $ smolnes <rom.nes>                                      */
#define z m(++St,1,0,0)                                                                      /*                                                          */
#define BD break;default:                                                                    /* Controls: DPAD=Arrow keys B=Z A=X Start=Enter Select=Tab */
#define g(x)m(St--,1,x,1);                                                                   /*                                                          */
#define x(a,b,c)W a:case b:case c:                                                           /************************************************************/
typedef uint8_t u;typedef uint16_t U;u*E,*C,K[4],M[8],e=14,Mb=12,S[8],MB,MI,ML,A,X,Y,P=4,St=~2,h,l,i,j,L,r,v,t,pm,p,H,pb,pw,fx,o,N,nb,pl,ph,vr[_*2],Pr[_],D
,ra[_*8],cr[_*8],pr[_*8],oa[_],bm[]={128,64,1,2,1,0,0,1,4,0,0,4,0,0,64,0,8,0,0,8},ks,mb,F,I,c0,c1,J,b[_*_],*kb;U y,T,V,f,d,at,a,s,c,fb[_*_];u*q(U a){Q&C[M[
a>>Mb]<<Mb|a&(1<<Mb)-1];}u*nt(U a){Q&vr[!D?a%_:D==1?a%_+_:D==2?a&2047:a/2&_|a%_];}u m(u l,u h,u v,u w){U a=h<<8|l;Z(h>>=4){x(0,1,~0)Q+w?ra[a]=v:ra[a];x(2,3
,~2)l&=7;k(l==7){t=pb;u*E=V<_*8?!w||C==cr?q(V):&t:V<16128?nt(V):Pr+(u)((V&19)==16?V^16:V);w?*E=v:(pb=*E);V+=p&4?32:1;V%=16*_;Q+t;}k(w)Z(l){W+0:T=T&62463|v%
4*_;p=v;W+1:pm=v;W+5:T=(pw^=1)?fx=v&7,T&~31|v/8:T&35871|v%8<<12|v*4&992;W+6:T=(pw^=1)?(u)T|v%64<<8:(V=T&~255|v);}k(l==2)Q t=H&224,H&=127,pw=0,t;W+4:k(l==20
&&w)for(f=256;f--;)oa[f]=m(f,v,0,0);for(t=0,h=8;h--;)t=t*2+kb[(u[]){27,29,43,40,82,81,80,79}[h]];Q+l==22?w?ks=t:(t=ks&1,ks/=2,t):0;x(6,7,~6)a&=8191;Q+w?pr[
a]=v:pr[a];                                                                                                                                     BD k(w)Z(b[
6]>>4){W+7:  D=!(v/16);*K=v=v%8*2;K[1]=v+1;W+4:Z(h>>1){W+4:*(a&1?&S[MB&7]:&MB)=v;t=MB>>5&4;for(l=4;l--;)M[l+t]=S[l/2]&~!(l%2)|l%2,M[4+l-t]=S[2  +l];K[t=MB/
32&2]=S[6];  K[1]=S[7];K[2-t]=b[4]*2-2;K[3]=b[4]*2-1;W+5:~a&1?(D=2+v%2):0;W+6:~a&1?(ML=v):0;W+7:MI=a&1;}W+3:*M=v=v%4*2;M[1]=v+1;W+2:*K=v&31;W+  1:v&128?F=0
,mb=5,I|=12  :(F=F/2|v<<4&+16,!--mb)?mb=5,t=a>>13,*(t==4?D=F&3,&I:t==5?&c0:t==6?&c1:&J)=F,*M=c0&~!(I&16),M[1]=I&16?c1:c0|1,t=I/4&3,*K=t==2?0:t  ==3?J:J&~1,
K[1]=t==2?J  :t==3?b[4]-1:J|1:0;}Q+E[(K[h-8>>e-12]&(b[                                       4]<<14-e)-1)<<e|a&(1<<e)-1];}Q~0;}u R(){v=m(l,h,0  ,0);!++l?++
h:0;Q+v;;}u  n(u v){Q P=P&125|v&128|!v*2;}int main(int                                       w,char**av){SDL_RWread(SDL_RWFromFile(av[1],"rb")  ,b,_*_,1);E
=b+16;K[1]=  b[4]-1;C=b[5]?E+((K[1]+1)<<14):cr;M[1]=(b[5]?b[5]:1)*2-1;D=3-b[6]%2;b[6]/16==4?m(0,128,0,1),e--,Mb-=2:0;l=+m(~3,~0,w=0,0);h=m(~2,  ~0,0,0);SD\
L_Init(32);  kb=(SDL_GetKeyboardState(0));void*re=SDL_CreateRenderer(SDL_CreateWindow(("smolnes"),0,0,_,840,4),-1,4),*tx=SDL_CreateTexture(re,  357896194,1
,256,224);Y  :c=L=0;k(N)             G N;Z((o=R())&31)                                       {W+0:k(o&128){R();L=1;G n;}Z(o>>5){W+0:!++l?++h:0  ;N:g(h)g(l)
g(P|32)l=m(  N=~1-(N&4),  ~0,0,0);h  =m(N+1,~0,0,0);N=                                       0;c++;W+1:r=R();g(h)g(l)h=R();l=r;W+2:P=~32&z;l=z  ;h=z;W+3:l=
z;h=z;!++l?  ++h:0;}c+=4  ;W+16:R()  ;!(P&bm[o>>6&3])^o/32&1?((r=l+(int8_t)v>>8)?h+=r,c++:0),c++,l+=(int)v:0;O(+8)Z(o>>=+4){W+0:g(P|48)c++;W+2  :c+=2;P=~16
&z;BD P=P&~  bm[o+3]|bm[  o+4];W+4:  g(A)c++;W+6:n(A=z);c+=2;W+8:n(--Y);W+9:n(A=Y);W+10:n(Y=A);W+12:n(++Y);W+14:n(++X);}O(10)Z(o>>4){W+8:n(A=X  );W+9:St=X;
W+10:n(X=A)  ;W+          11:n(X=St          );W+12:n(                                       --X);W+14:BD L=1;v=A;G n;}W+1:R();v+=X;i=m(v,0,0,  0);j=m(v+1,
0,0,0);c+=4  ;;G  o;x(4,5,6)i   =R();c++;j=  0;G o;x(2                                       ,9,~2)R();L=1;G n;x(12,13,14)i=R();j=R();c+=2;G o  ;W+17:i=m(R
(),0,0,0);j  =m(  v+1,0,0,0       );v=Y;c++  ;t=o==145;G A;x(20,21,22)i=R()+((o&214)==150?Y:X);j=+0                 ;                 c+=2;G o  ;W+5*5:i=R(
);j=R();v=Y  ;t=  o==153;;G       A;x(28,29  ,30)i=R();j=R();v=o==190?Y:X;t=o==157||o%16==14&&190!=  o;A:j+=r=255<  i  +v;i+=v;c+=2+  t|r;o:76  !=o&&(o&224
)!=128?v=m(  i,j  ,0,0):0;n       :Z(o&243)  {O(1)n(A                                         |=v);  O(33     )n(A  =  A&v)     ;O(5  *13)n(A=  A^v);O(225)
v=~v;G a;O(  97)  a:f=A+v+P%2   ;P=P&~65|f>  255|((A^  f)&(v^f)&128)/2;n(A=f);O(2)r=v*2;P=P&  ~1|v/  2/         64  ;  ;G         m;  O(34)r=v  *2|P &1;P=P
&~1|v/128;G  m;O          (6*11)r=v          /2;P=P&~  1|v&1        ;G m;O(98)        r=v/2|  P<<7;  P           =  P  &           ~  1|v&1;;G  m;O(194)r=v
-1;G m;O(2*  113)r=v+1;m  :n(r);L?A  =r:(c+=2,m(i,j,r  ,1))          ;W+32:P=          P&61|  v&192  |!         (A  &  v)         *2  ;W+64:l=  i;h=j;c--;W
96:l=v;h=m(  i+1,j,0,0);  c++;O(160  )n(Y=v);O(161)n(  A=v);        O(162)n(X=        v);O(2  *64)r  =Y;G     s;O(  1  +128     )r=A  ;G s;O(2  *65)r=X;s:m
(i,j,r,1);O  (192)r=Y;;G  c;O(193)r  =A;G c;O(224)r=X  ;c:P=P&~1|r>=v;n(r-v);}}for(t=c*3+6;t  --;){  k(pm&24){k(240 >  y){k(d<256||d  >319){Z(  d&7){W+1:nb
=*nt(V);W+3  :at=(*nt(V&             3072|V>>4&56|960                                         |V/4&                 7                 )>>(V>>5  &2|V/2&1)*2
)%4*21845;W  5:pl=*q(p<<8&_*4|nb<<4|V>>12);W+7:ph=*q(p<<8&_*4|nb<<4|V>>12|8);V=V%32==31?V&~31^_:V+1;}k(y<240&&d<256){u C=a>>14-fx&2|s>>15-fx&1  ,P=w>>28-fx
*2&12;k(pm&  16)for(u*s=oa;s<oa+256;s+=4){U h=p&32?16:8,X=d-s[3],Y=y-*s-1,i=X^(s[2]&64?0:7),j=Y^(s[2]&128?h-1:0);k(X<8&&Y<h){U t=s[1],a=p&32?t  %2<<12|(t&~
1)<<4|(j&8)  *2|j&7:(p&8)<<9|t<<4|j&7,c=*q(a+8)>>i<<1&2|*q(a)>>i&1;k(c){!(s[2]&32&&C)?C=c,P=16|s[2]*4&12:0;s==oa&&C?H|=64:0;G X;}}}X:fb[y*256+  d]=(U[64]){
25356,34816                                                                                                                                     ,39011,308\
54,24714,4107,106,2311,2468,2561,4642,6592,20832,0,0,0,44373,49761,55593,51341,43186,18675,434,654,4939,5058,3074,19362,37667,0,0,0,~0,~819,64497,64342,62\
331,43932,23612,9465,1429,1550,20075,36358,52713,16904,0,0,~0,~328,~422,~452,~482,58911,50814,42620,40667,40729,48951,53078,61238,44405}[Pr[C?P|C:0]];}336>
d?a*=2,s*=2,w*=4:0;(d%8==7)?a|=ph,s|=pl,w|=at:0;}d==256?V=((V>>12&7)!=7?V+_*4:(V&992)==928?V&35871^_*2:(V&992)==992?V&35871:V&35871|V+32&992)&~1055|T&1055:
0;d==261&&MI&&!ML--?N=1:0;}y==261&&d>279&&d<305?V=V&33823|T&31712:0;}k(y==241&&d==1){p&128?N=4:0;H|=128;SDL_UpdateTexture(tx,0,fb+_*2,512);;SDL_RenderCopy(
re,tx,0,0);SDL_RenderPresent(re);for(SDL_Event ev;SDL_PollEvent(&ev);)k(ev.type==256)Q+0;}y==261&&d==1?H=0:0;++d==341?d=0,y++,y%=262:0;}G Y;}U __by_binji_;
