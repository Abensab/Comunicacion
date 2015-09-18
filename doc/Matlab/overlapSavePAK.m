function [ y ] = overlapSavePAK( x, h, size )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here

% Poner los vectores de entrada en columnas
x = x(:);
h = h(:);

% Calcular longitudes

% Longitud de X
Lx = length(x);

% Longitud de h
Lh = length(h);

% Longitud de la ventana
% (Maximo entre 512 o la potencia de 2 inmediatamente superior al doble de
% la longitud de h) (tambien se puede utilizar nextpow2)
L = max(size, 2^(floor(log2(Lh))+1));

% Pre-calculamos la FFT de H para la convolución circular
H = fft(h, L);

% Punteros iniciales
puntero_inicio_x    = 1 - (Lh-1);
puntero_fin_x       = puntero_inicio_x + (L-1);
puntero_inicio_y    = 1;
puntero_fin_y       = puntero_inicio_y + (L-1)-(Lh -1);

while (puntero_inicio_x <= Lx) % Hasta que el trozo de x no se haya salido 
%del todo

    % Preparamos el trozo de X
    num_ceros_iniciales = -(puntero_inicio_x-1);
    num_ceros_finales   = puntero_fin_x-Lx;
    indice_inicio_x     = max(1,puntero_inicio_x);
    indice_fin_x        = min(Lx, puntero_fin_x);
    x_trozo             = [zeros(num_ceros_iniciales,1);
        x(indice_inicio_x:indice_fin_x);
        zeros(num_ceros_finales,1)];
    
    % Hacemos la convolucion circular modulo L
    X_trozo = fft(x_trozo);
    Y_trozo = X_trozo.*H;
    y_trozo = ifft(Y_trozo);
    
    % Quitamos la parte de Y_trozo que no interesa
    % es decir, las (Lh-1) primeras muestras
    % que son el resultado erroneo de la 
    % convolucion circular.
    y_trozo = y_trozo(Lh:end);
    
    % Insertamos el trozo de y en el lugar correspondiente de y
    y(puntero_inicio_y:puntero_fin_y,1) = y_trozo;
    
    % Actualizo los punteros para el trozo siguiente
    puntero_inicio_x    = (puntero_fin_x+1)-(Lh-1);
    puntero_fin_x       = puntero_inicio_x + (L-1);
    puntero_inicio_y    = (puntero_fin_y + 1);
    puntero_fin_y       = puntero_inicio_y + (L-1) - (Lh-1);
    
end

