function [y] = oversave_max (x, h)

disp('hola')
% Pasar los vectores x y h a vectores columna
x = x(:);
h = h(:);

% Longitud de h
P = length(h);

% Longitud de x
Lx = length(x);

% Longitud de la ventana
% (Maximo entre 512 o la potencia de 2 inmediatamente superior al doble de
% la longitud de h) (tambien se puede utilizar nextpow2)
L = max(512, 2^(floor(log2(P))+1));

% Pre-calculamos la FFT de H para la convolución circular
H = fft(h,L);

% Numero de iteraciones
N_iter = ceil(Lx/(L-(P-1))) + 1;

% Duracion de la salida y reserva de memoria
Final_length = (L-(P-1))*N_iter;
y = zeros(Final_length,1);

% Duracion de cada trozo filtrado válido:
Lv = L - (P-1);

% Añadimos ceros al principio
x = [zeros(P-1,1); x];

muestra_ini = 1;
for n = 1: N_iter
    
    muestra_fin = muestra_ini + L - 1;
    
    %Al coger el último trozo de longitud L es posible que tengamos que
    %añadir ceros al final de la secuencia
    if muestra_fin> Lx
     x = [x; zeros(muestra_fin-Lx,1)];
    end
    
    % Tomamos el trozo de x correspondiente
    chunk_x = x(muestra_ini:muestra_fin);
    % FFT para convolución circular
    cX = fft(chunk_x);
    % Trozo de señal filtrado
    chunk_f = ifft(cX.*H);
    
    y((n-1)*Lv + 1: n*Lv) = chunk_f(P: end);
    
    muestra_ini = muestra_fin - P + 2;    
end

%Sabemos que la longitud de la convolucion lineal es Lx + P -1, así que
%añadimos ceros al final para que coincida con la instrucción conv()
y = [y; zeros(Lx+P-1 - length(y),1)];