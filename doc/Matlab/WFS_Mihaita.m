%% Filtro H, depende sólo de las frecuencias
c=343;
comp=sqrt(-1);
ang_max=90*(pi/180); % rad
ord=60;
FS=44100;
land=-1; % porque está fuera del array de altavoces


load H1.mat


%% Altavoces y fte
alt(1,1:8)=1.0182-(0.09:0.18:1.35)*cos(45*pi/180);
alt(2,1:8)=(0.09:0.18:1.35)*cos(45*pi/180);
alt(1,9:32)=0;
alt(2,9:32)=(1.0182)+(0.09:0.18:2*1.44+1.35);
alt(1,33:40)=(0.09:0.18:1.35)*cos(45*pi/180);
alt(2,33:40)=(1.0182+3*1.44)+(0.09:0.18:1.35)*cos(45*pi/180);
alt(1,41:48)=(1.0182)+(0.09:0.18:1.35);
alt(2,41:48)=(2*1.0182+3*1.44);
alt(1,49:56)=(1.0182+1.44)+(0.09:0.18:1.35)*cos(45*pi/180);
alt(2,49:56)=(2*1.0182+3*1.44)-(0.09:0.18:1.35)*cos(45*pi/180);
alt(1,57:80)=(2*1.0182+1.44);
alt(2,57:80)=(1.0182)+(0.09:0.18:2*1.44+1.35);
alt(1,81:88)=(2*1.0182+1.44)-(0.09:0.18:1.35)*cos(45*pi/180);
alt(2,81:88)=(1.0182)-(0.09:0.18:1.35)*cos(45*pi/180);
alt(1,89:96)=(1.0182+1.44)-(0.09:0.18:1.35);
alt(2,89:96)=0;
nalt=96;

%FUENTE
%fte=[3.8;3.2]; %fte 1
%fte=[1.75;7]; %fte 2
%fte=[2;-0.5]; %fte 3
%fte=[-0.2;1.2]; %fte 4
fte=[-1.0;0.0];
nfte=1;

figure;plot(alt(1,:),alt(2,:),'*'); 

% for tt=1:length(alt(1,:))
%     if 
% 
% 
% end


hold on; plot(fte(1),fte(2),'+');

%% Amplitud
% Datos de los angulos de cada array (hacia donde estan orientados), tal y
% como están definidos arriba.
% Tomamos como 0 º el que está arriba, sobre el eje X orientado hacia
% valores negativos de X. Lo escribimos en radianes.
% tecta=45*pi/180; %el array forma 45º
tecta=[ones(1,8)*135,ones(1,24)*90,ones(1,8)*45,ones(1,8)*0,ones(1,8)*-45,ones(1,24)*-90,ones(1,8)*-135,ones(1,8)*180];

fuente=fte;
x = fuente(1);
y = fuente(2);
%TIENES QUE PONER ESTO!!!!!. el tecta y el alfa lo dejamos porque son para ver quien escucha ycual no. Pero a ti te te daigual.
%Cálculo del ángulo fte-altavoz
difX = alt(1,:)-x;
difY = alt(2,:)-y;
alfa = atan2(difY,difX);      % Ángulo
alfa=(alfa.*180/pi)+90-tecta;
[parray,pos]=find(((alfa<90)&(alfa>-90))|((alfa<450)&(alfa>270)));
parray_act=pos;

%alfa(1:8) = pi/2 - tecta + alfa(1:8); % Angulos del lateral izquierdo
%alfa(25:32) = pi/2 - tecta + alfa(25:32); % Angulos del lateral derecho % derecho --> desactivado:
%Cálculo de la distancia fte-altavoz
r = sqrt(difX.^2 + difY.^2);                      % Distancia entre la fuente y cada altavoz
%alfa=difX./cos(tecta-pi/2)./r;
%Amplitud
% Para la distancia entre el altavoz y la línea imaginaria interior. Vamos
% a suponer una línea interior con la misma forma que el array. Y que está
% situada a 2/3 del array, contando desde el array hasta el punto central.
% Se tiene que calcular para todo, pero de momento, para una fuente situada
% a la izquierda del array, para los altavoces que están activos (2,3 y 4),
% calculamos la distancia para el centro de la sala y sería: 
% 1.44/2+1.44*cos(45*pi/180)
%r0=ones(1,nalt) *2.88 * (1/2);
r0=ones(1,nalt) * (1.44/2+1.44*cos(45*pi/180));
s0=abs(r);%.*sin(alfa);
A=sqrt(r0./(r0+s0));
an=A.*cos(alfa*(pi/180))./(sqrt(r));
%%%%%AQUI NOS QUEDAMOS......................................

%% Retardo
tn=-land.*(FS*(r/c)); % delay factor, for the n-loudspeaker



% t0=round(abs(min(tn)));
% tmax=round(abs(max(tn(parray_act))));
% 
% %% Filtro salida
% sn=1;
% paux_sign=conv(sn,H_1); %length=length(sn)+length(h)-1
% 
% 
% delta=zeros(512-ord,1);
% pout_sign_aux=zeros(512,nalt);
% for i=parray_act
%     paux_out=paux_sign*an(i);
%     delta=zeros(512-ord,1);
%     %delta(tn(i))=1; 
%     delta(round(tn(i)))=1; 
%     pout_sign_aux(:,i)=conv(paux_out,delta);
% end;
