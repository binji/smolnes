#include                                                                          <SDL2/SDL.h> /************************************************************/
#include                                                                            <stdint.h> /*             SMOLNES by Ben Smith (Oct 2022)              */
#define                                                                                 _ 1024 /* -------------------------------------------------------- */
#define                                                                                 G goto /*                                                          */
#define                                                                               B break; /* A NES emulator that can play mapper 0/1/2/3/7 games.     */
#define                                                                               Q return /* Requires gcc/clang and Linux/macOS.                      */
#define                                                                               Z switch /*                                                          */
#define                                                                               W B case /* $ cc smolnes.cc -O2 -lSDL2 -o smolnes                    */
#define                                                                         z m(++S,1,0,0) /* $ smolnes <rom.nes>                                      */
#define                                                                      g(x)m(S--,1,x,1); /*                                                          */
#define                                                                     SD(x,y) SDL_##x##y /* Controls: DPAD=Arrow keys B=Z A=X Start=Enter Select=Tab */
#define                                                                     O(x)W x:case x+16: /************************************************************/
typedef uint8_t u;typedef uint16_t U;int rg[64]={25356,34816,39011,30854,24714,4107,106,2311,2468,2561,4642,6592,20832,0,0,0,44373,49761,55593,51341,43186,1\
8675,434,654,4939,5058,3074,19362,37667,0,0,0,~0,~819,64497,64342,62331,43932,23612,9465,1429,1550,20075,36358,52713,16904,0,0,~0,~328,~422,~452,~482,58911,\
50814,42620,40667,40729,48951,53078,61238,44405},y;u*E,*C,K[2],M[2],A,X,Y,P=4,S=~2,h,l,i,j,L,r,v,x,t,t2,pm,p,H,pb,pw,fx,o,N,nb,pl,ph,vr[_*2],Pr[64],ra[_*8],D
,cr[_*8],pr[_*8],oa[256],bm[]={128,64,1,2,1,0,0,1,4,0,0,4,0,0,64,0,8,0,0,8},ks,mb,F,I,c0,c1,J,b[_*_],*k;U T,V,f,d,e,a,s,c,fb[61440];u*q(U a){Q&C[M[a>>12]<<12
|a&4095];}u*nt(U a){Q&vr[!D?a%_:D==1?a%_+_:D==2?a&2047:a/2&_|a%_];}u m(u l,u h,u v,u w){U a=h<<8|l;Z(h>>4){W(0)...1:Q+w?ra[a]=v:ra[a];W(2)...3:l&=7;if(l==7){
t=pb;u*E=V<_                                                                                                                                     *8?!w||C==cr
?q(V):&t2:V<  16128?nt(V):Pr+(u)((V&19)==16?V^16:V);w?*E=v:(pb=*E);V+=p&4?32:1;V%=16*_;Q+t;}if(w)Z(l){W+0:p=v;T=T&62463|v%4<<10;W+1:pm=v;W+5:T=  (pw^=1)?fx=v
&7,T&~31|v/8  :T&35871|v%8<<12|v*4&992;W+6:T=(pw^=1)?(u)T|v%64<<8:(V=T&~255|v);}if(l==2)Q t=H&224,H&=127,pw=0,t;W+4:if(w&&l==20)for(f=256;f--;)  oa[f]=m(f,v,
0,0);Q+l==22  ?w?ks=(k[79]*8+k[80]*4+k[81]*2+k[82])*16+k[40]*8+k[43]*4+k[29]*2+k[27]:(t=ks&1,ks/=2,t):0;W(6)...7:Q+w?pr[a&8191]=v:pr[a&8191];W(  8)...15:if(w
)Z(b[6]>>4){  W+7:D=!(v/16);*K=v=v%8*2;K[1]=v+1;W+3:*M=                                       v=v%4*2;M[1]=v+1;W+2:*K=v&31;W+1:v&128?mb=5,F=0,I  |=12:(F=F/2|
v<<4&+16,!--  mb)?mb=5,t=a>>13,*(t==4?D=F&3,&I:t==5?&c0                                       :t==6?&c1:&J)=F,*M=c0&~!(I&16),M[1]=I&16?c1:c0|1,  t=I/4&3,*K=t
==2?0:t==3?J  :J&~1,K[1]=t==2?J:t==3?b[4]-1:J|1:0;}Q+E[K[(a>>14)-2]<<14|a&16383];}Q~0;}u R(){v=m(l,h,0,0);!++l?++h:0;Q+v;}u n(u v){Q P=P&~130|v  &128|!v*2;}\
int main(int  w,char**av){SDL_RWread(SDL_RWFromFile(av[1],"rb"),b,_*_,1);E=b+16;K[1]=b[4]-1;C=b[5]?E+((K[1]+1)<<14):cr;M[1]=(b[5]?b[5]:1)*2-1;D  =3-b[6]%2;l=
+m(~3,~0,w=0  ,0);h=m(~2,             ~0,0,0);SDL_Init(                                       32);void*re=SDL_CreateRenderer(SDL_CreateWindow((  "smolnes"),0
,0,_,840,4),  -1,4),*tx=(  SD(Creat,  eTexture))(re,555                                       *644858+4,1,256,224);k=(SDL_GetKeyboardState(0));  for(;;){c=L=
0;if(N)G N;Z  ((o=R())&31  ){W+0:if(  o&128){R();L=1;G n;}Z(o>>5){W+0:!++l?++h:0;N:g(h)g(l)g(P|32)l=m(N=~1-N*4,~0,0,0);h=m(N+1,~0,0,0);c++;N=0;  W+1:r=R();g(
h)g(l)h=R();  l=r;W+2:P=z  &~32;l=z;  h=z;W+3:l=z;h=z;!++l?++h:0;}c+=4;W+16:R();!(P&bm[o>>6&3])^o/32&1?((x=l+(int8_t)v>>8)?h+=x,c++:0),c++,l+=(  int)v:0;O(+8
)Z(o>>=+4){W  0:g          (P|48)c++          ;W+2:P=z&                                       ~16;c+=2;B;default:P=P&~bm[o+3]|bm[o+4];W+4:g(A)c  ++;W+6:n(A=z
);c+=2;W+8:n  (--  Y);W+9:n(A=   Y);W+10:n(Y  =A);W+12:                                       n(++Y);W+14:n(++X);}O(10)Z(o>>4){W+8:n(A=X);W+9:S  =X;W+10:n(X=
A);W+11:n(X=  S);  W+12:n(--       X);W+14:B  default:L=1;v=A;G n;}W+1:R();v+=X;i=m(v,0,0,0);j=m(v+1                 ,                 0,0,0);c  +=4;G o;W(4)
...6:i=R();j  =0;  c++;G o;W       2:case 9:  R();L=1;G n;W(12)...14:i=R();j=R();c+=2;G o;W+17:i=m(R  (),0,0,0);j=m  (  v+1,0,0,0);v=  Y;c++;t=  o==145;G A;W
20 ...22:i=   R()  +((o&214)       ==150?Y:X  );j=+0;G                                         o;W+5  *5:i    =R();  j  =R();    c+=2  ;v=Y;t=o  ==153;G A;W(
28)...30:i=R  ();  j=R();v=o==   190?Y:X;t=o  ==157||o  %16==14&&o!=190;A:j+=x=i+v>255;i+=v;c  +=2+t  |x         ;o  :  (o         &2  *112)!=2  *64&&o!=76?v
=m(i,j,0,0):  0;n          :Z(o&243)          {O(1)n(A  |=v);        O(33)n(A&=        v);O(5  *13)n  (           A  =  A           ^  v);O(225  )v=~v;G a;O(
97)a:f=A+v+P  %2;P=P&~65|  f>255|(~(  A^v)&(v^f)&128)/  2;n(          A=f);O(2          )r=v*  2;P=P  &~         1|  v  /2         /2  /32;G m;  O(34)r=v*2|P
&1;P=P&~1|v/  128;G m;O(6  *11)r=v/2  ;P=P&~1|v&1;G m;  O(98)        r=v/2|P<<7        ;P=P&~  1|v&1  ;G m     ;O(2  *  97)r     =v-1  ;G m;O(2  *113)r=v+1;m
:n(r);L?A=r:  (c+=2,m(i,j  ,r,1));W+  32:P=P&61|v&192|  !(A&v)*2;W+64:l=i;h=j;c--;W+96:l=v;h=  m(i+1  ,j,0,0);c++;O  (  160)n(Y=v);O(  161)n(A=  v);O(162)n(X
=v);O(128)r=  Y;G s;O(129             )r=A;G s;O(130)r                                         =X;s:                 m                 (i,j,r,1  );O(192)r=Y;
G c;O(193)r=  A;G c;O(224)r=X;c:P=P&~1|r>=v;n(r-v);}}for(t=c*3+6;t--;){if(pm&24){if(y<240){if(d<256||d>319){Z(d&7){W+1:nb=*nt(V);W+3:e=(*nt(960  |V&3072|V>>4
&56|V/4&7)>>  (V>>5&2|V/2&1)*2)&3;e|=e*4;e|=e<<4;e|=e<<8;W+5:pl=*q(p<<8&_*4|nb<<4|V>>12);W+7:ph=*q(p<<8&_*4|nb<<4|V>>12|8);V=V%32==31?V&~31^_:V  +1;}if((U)y<
240&&d<256){                                                                                                                                     u C=a>>14-fx
&2|s>>15-fx&1,P=w>>28-fx*2&12;if(pm&16)for(u*s=oa;s<oa+256;s+=4){U h=p&32?16:8,X=d-s[3],Y=y-*s-1,i=X^(s[2]&64?0:7),j=Y^(s[2]&128?h-1:0);if(X<8&&Y<h){U t=s[1]
,a=p&32?t%2<<12|(t&~1)<<4|(j&8)*2|j&7:(p&8)<<9|t<<4|j&7,c=*q(a+8)>>i<<1&2|*q(a)>>i&1;if(c){!(s[2]&32&&C)?C=c,P=16|s[2]*4&12:0;s==oa&&C?H|=64:0;B}}}fb[y*256+d
]=rg[Pr[C?P|C:0]];}d<336?a*=2,s*=2,w*=4:0;(d%8==7)?a|=ph,s|=pl,w|=e:0;}d==256?V=((V&7<<12)!=7<<12?V+_*4:(V&992)==928?V&35871^_*2:(V&992)==992?V&35871:V&35871
|V+32&992)&~1055|T&1055:0;}y==-1&&d>279&&d<305?V=V&33823|T&31712:0;}if(y==241&&d==1){p&128?N=1:0;H|=128;SDL_UpdateTexture(tx,0,fb+_*2,512);SDL_RenderCopy(re,
tx,0,0);SDL_RenderPresent(re);for(SDL_Event ev;SDL_PollEvent(&ev);)if(ev.type==256)Q+0;}y==-1&&d==1?H=0:0;++d==341?d=0,y=y==260?-1:y+1:0;}}}U(made_by_binji);
