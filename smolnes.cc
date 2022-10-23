#include                                                                                             <SDL2/SDL.h> /*****************************************/
#include                                                                                             <sys/mman.h> /*    SMOLNES by Ben Smith (Oct 2022)    */
#include                                                                                               <unistd.h> /* ------------------------------------- */
#include                                                                                                <cstdint> /* A NES emulator that can only play a   */
#include                                                                                                <fcntl.h> /* few mapper 0 and mapper 1 games.      */
#define                                                                                                    G goto /* Requires gcc/clang and Linux/macOS.   */
#define                                                                                                  B break; /*                                       */
#define                                                                                                  Q return /* $ cc smolnes.cc -O2 -lSDL2 -o smolnes */
#define                                                                                                  Z switch /* $ smolnes <rom.nes>                   */
#define                                                                                                  W B case /*                                       */
#define                                                                                               PL m(++S,1) /* Controls:                             */
#define                                                                                        O(x)W x:case x+16: /*    Up/Down/Left/Right=Arrow keys      */
#define                                                                                        SD(x,y) SDL_##x##y /*    B=Z, A=X, Start=Enter, Select=Tab  */
#define                                                                                       PH(x) m(S--,1,x,1); /*****************************************/
using u=uint8_t;  using U=uint16_t; int p4[64]={ 25356,34816,39011, 30854,24714,4107,106,2311, 2468,2561,4642,6592,20832,0,0,0,44373,49761,55593,51341,43186,
18675,434,654,4939,5058,3074,19362,37667,0,0,0,~0,~819,64497,64342,62331,43932,23612,9465,1429, 1550,20075,36358,52713,16904,0,0,~0,~328,~422,~452,~482,58911
,50814,42620,40667,40729,48951,53078,61238,44405},y,_,PM[]={~1,0,~0,1,~4,0,~0,4,~0,0,~64,0,~8,0,~0,8};u*q,*K,*J[2],*L[2],A,X,Y,P=4,S=253,h,l,i,j,z,r,v,x,t,t2
,pm,p,g,pb,pw,I,o,N,nt,pl,ph,vr[2048],pr[64],ra[8192],cr[8192],PR[8192],M[256],bm[]={128,64,1,2},kz,e,mb,D,E,c0,c1,F;U T,V,f,d,at,sh,sl,c,fb[61440];u const*k
;u ad(u v){x=i+v>255;j+=x;Q i+=v;}u&H(U a){Q L[a>>12][a&4095];} u&NT(U a){Q vr[!e?a&1023:e==1?a%1024+1024:e==2?a&2047:a/2&1024|a&1023];}u m(u l,u h=0,u v=0,u
w=0){U a=h<<                                                                                                                                     8|l;Z(h>>4){
W 0 ...1:Q w  ?ra[a]=v:ra[a];W 2 ...3:l&=7;if(l==7){t=pb;u&q=V<8192?!w||K==cr?H(V):t2:V<16128?NT(V):pr[((V&19)==16?V^16:V)&255];w?q=v:pb=q;V+=(  p&4 ?32 :1)&
16383 ;Q t;}  if(w)Z(l){W 0:p=v;T=T&62463|v%4<<10;W 1:pm=v;W 5:T=(pw^=1)?I=v&7,T&~31|v/8:T&35871|v%8<<12|(v&248)*4;W 6:T=(pw^=1)?T&255|v%64<<8:  V=T&~255|v;Q
0;}if(l==2)Q  t=g&224,g&=~80,pw=0,t;W 4:if(w&&l==20)for(f=256;f--;)M[f]=m(f,v);else if(l==22)Q w?kz=k[79]*128+k[80]*64+k[81]*32+k[82]*16+k[40]*  8+k[43]*4+k[
29]*2+k[27]:  (t=kz&1, kz/=2,t);Q 0;W 6 ...7:Q w?PR [a&                                       8191]=v:PR[a&8191];W 8 ...15:if(w)if(v&128)mb=5,D =0,E|=12;else
if(D=D/2|v<<  4&16,!--mb)mb=5,t=a>>13,(t==4?e=D&3,E:t==                                       5?c0:t==6?c1:F)=D,L[0]=K+(c0&~!(E&16))*4096,L[1]=  K+(E&16 ?c1:
c0|1)*4096,t  =E/4&3,J[0]=q+((t==2?0:t==3?F:F&~1)<<14),J[1]=q+((t==2?F:t==3?q[-12]-1:F|1)<<14);Q J[(a>>14)-2][a&16383];}Q~0;}u R(){v=m(l,h);!++  l?++h:0;Q v;
}u n(u v) {Q  P=P&~130|v&128|!v*2;}int main(int,char**F){J[0]=q=(u*)mmap(0,1<<20,1,1,open(F[1],0),0)+16;J[1]=q+(q[-12]-1<<14);L[0]=K=q[-11]?J[1  ]+16384:cr;L
[1]=K+q[-11]  *8192-4096;             e=!(q[-10]&1)+2;l                                       =m(~3,~0);h=m(~2,~0);SDL_Init(32);auto*D=SD(Crea,  teRenderer)(
SD(CreateWi,  ndow)("smol  nes",0,0,  2<<9,840,4),-1,4);                                      auto*E=SDL_CreateTexture(D,357896194,1,256,224);k  =SD(GetKeyb,
oardState)(0  );for(;;){c  =z=0;if(N  )G N;Z((o=R())&31){W 0:if(o&128){R();z=1;G n;}Z(o>>5){W 0:!++l?++h:0;N:PH(h)PH(l)PH(P|32)l=m(~1-N*4,~0);h  =m(~0-N*4,~0
);c++ ;N=0;W  1:r=R();PH(  h)PH(l)h=  R();l=r;W 2:P=PL&~32;l=PL;h=PL;W 3:l=PL;h=PL;!++l?++h:0;B}c+=4;W 16:R();if(!(P&bm[o>>6&3])^(o>>5)&1)(x=l+  (int8_t)v>>8
)?h+=x,c++:0  ,c++         ,l+=(int)          v;O(8)Z(o                                       >>=4){W 0:PH(P|48)c++;W 2:P=PL&~16;c+=2;W 4:PH(A)  c++;W 6:n(A=
PL);c+=2;W 8  :n(  --Y);W 9:n(   A=Y);W 10:n  (Y= A) ;W                                       12:n(++Y);W 14:n(++X);B default:P=P&PM[o-1]|PM[o]  ;B}O(10 )Z(o
>>4){W 8:n(A  =X)  ;W 9:S=X;       W 10:n(X=  A);W 11:n(X=S);W 12:n(--X);W 14:B default:z=1;v=A;G n;                 }                 W 1:R();  v+=X;i=m(v);
j=m(v+1);c+=  4;G  o;W 4 ...       6:i=R();j  =0;c++;G o;W 2:case 9:R();z=1;G n;W 12 ...14:i=R();j=R  ();c+= 2;G o;  W  17:i=m(R());j  =m(v+1);  ad(Y);c+=3+(
o==145||x);G  o;W  20 ...22:       i=R()+((o  &214 )==                                         150?Y  :X);     j=0;  c  +=2;     G o;  W  25:i=  R();j=R();ad
(Y);c +=2+(o  ==   153||x);G o   ;W 28 ...30  :i=R();j  =R();ad(o==190?Y: X);c+=2+(o==157||o%  16 ==  14         &&  o  !=        190  ||x);o:(  o&224) !=128
&&o!=76?v=m(  i,j          ):0;n:Z(o          &243){O(  1)n(A        |=v);O(33)        n(A&=v  ); O(  65          )  n  (           A  ^=v );O(  225)v=~v;G a
;O(97)a:f=A+  v+(P&1);P=P  &~ 65|f >  255|(~(A^v)&(v^f  )&128         )/2;n(A=          f);O(  2)r=v  *2;        P=  P  &~         1|  v>>7;G m  ;O(34)r=v*2|
P&1;P=P&~1|v  >>7 ;G m;O(  66)r=v/2;  P=P&~1|v&1;G m;O  (98)r=       v/2|P<<7;P        =P&~1|  v&1;G  m;O(     194)  r  =v-1     ;G m  ;O(226)r  =v+1;m:n(r);
z?A=r:(c+=2,  m(i,j,r,1))  ;W 32:P=P  &61|v&192|!(A&v)  *2;W 64:l=i;h=j;c--;W 96:l=v;h  =m(i+  1,j);  c++;O(160)n(Y  =  v);O(161)n(A=  v);O(162  )n (X=v) ;O(
128)r=Y;G s;  O(129)r=A;G             s;O(130)r=X;s:m(                                         i,j,r                 ,                 1);O(192  )r= Y;G c;O(
193)r=A;G c;  O(224)r=X;c:P=P&~1|r>=v;n(r-v);B}}for(t=c*3+6;t--;){if(pm&24){if(y<240){if(d< 256||d>319){Z(d&7){W 1:nt=NT(V);W 3:at= (NT((960|V&  3072|V>>4&56
|V/4&7))>>(V  >>5&2|V/2&1)*2)&3;at|=at*4;at|=at<<4;at|=at<<8;W 5:pl=H(p<<8&4096|nt<<4|V>>12);W 7:ph=H(p<<8&4096|nt<<4|V>>12|8); V=(V&31)==31?V&  ~31^1024:V+1
;B}if ((U)y<                                                                                                                                     240&&d<256){
u c=sh>>14-I&2|sl>>15-I&1,P=_>>28-I*2&12;if(pm&16)for(u*s=M;s<M+256;s+=4){U h=p&32?16:8,x=d-s[3],Y=y-*s-1,i=x^(s[2]&64?0:7),j=Y^(s[2]&128?h-1:0);if(x<8&&Y<h)
{U t=s[1],a=p&32?t%2<<12|(t&~1)<<4|(j&8)*2|j&7:(p&8)<<9|t<<4|j&7,S=H(a+8)>>i<<1&2|H(a)>>i&1;if(S){!(s[2]&32&&c)?c=S,P=16|s[2]*4&12:0;s==M&&c?g|=64:0;B}}}fb[y
*256+d]=p4[pr[c?P|c:0]];}d<336? sh*=2,sl*=2,_*=4:0;d%8==7?sh|=ph,sl|=pl,_|=at:0;}d==256?V=((V&7<<12)!=7<<12? V+4096:(V&992)==928?V&35871^2048:(V&992)==992?V&
35871:V&35871|V+32&992)&~1055|T&1055:0;}y==-1&&d>279&&d<305?V=V&33823|T&31712:0;}if(y==241&&d==1){p&128?N=1:0;g|=128;SDL_UpdateTexture(E,0,fb+2048,512);SD(R,
enderCopy)(D,E,0,0);SD(Render,Present)(D);for(SDL_Event e;SDL_PollEvent(&e);)if(e.type==256)Q 0;}y==-1&&d==1?g=0:0;++d==341?d=0,y=y==260?-1:y+1:0;}}}U binji;
