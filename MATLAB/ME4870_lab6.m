clear ; clc ; close all ;
format shortG

rho = 1.2 ;              % Air density (kg/m^3)
c = 343 ;                % Speed of sound in air (m/s)
d = 2*0.0254 ;           % Diameter of tube (m)
L = (24-0.875)*0.0254 ;  % Geometric length of tube (m)
Le = L + 0.85*d/2 ;      % Effective length of tube (m)
A = pi/4*d^2 ;           % Cross-sectional area of tube (m^2)
Vv = (11.5*0.0254)^3 ;   % Volume of box (m^3)
m = rho*A*Le ;           % Mass of air in tube (kg)
kv = rho*c^2*A^2/Vv ;    % Effective stiffness of box volume (N/m)

%% 2 Elements
fprintf('2 Element Results:\n') ;
elements = 2 ;
% Effective stiffness element for 4 elements
k = rho*c^2*A/(Le/(2*elements)) ;   % (N/m)
% Mass matrix
M = m/elements*eye(elements) ;      % (kg)
% Stiffness matrix
K = [kv*k/(kv+k)+k/2 -k/2 ; ...
    -k/2 k/2+k] ;                   % (N/m)
% Compute eigenvalues
[~, D] = eig(M\K) ;
% Resonant frequencies
fn = 1/(2*pi)*sqrt(diag(D))         % (Hz)

% Calcualte pressure magnitude spectrum
F = zeros(elements,1) ; % Unity pressure input at first node
F(1) = A ;
f = 0:4:500 ;       % (Hz)
w = 2*pi*f ;        % (rad/s)
C = 50*M ; % Assumed damping matrix proportional to mass
for i = 1:length(f)
    x = (-w(i)^2*M + 1i*w(i)*C + K)\F ; % Calculate motion given force input
    p(i) = (k/2)*abs(x(1)-x(2))/A ; % Calculate pressure between nodes 1 and 2
end
%% 4 Elements
fprintf('4 Element Results:\n') ;
elements = 4 ;
k = rho*c^2*A/(Le/(2*elements)) ;   % (N/m)
M = m/elements*eye(elements) ;      % (kg)
K = [kv*k/(kv+k)+k/2 -k/2 0 0 ;...
    -k/2 k -k/2 0 ; ...
    0 -k/2 k -k/2 ; ...
    0 0 -k/2 k/2+k] ;               % (N/m)
[~, D] = eig(M\K) ;
fn = 1/(2*pi)*sqrt(diag(D))         % (Hz)

F = zeros(elements,1) ;
F(1) = A ;
f = 0:1:500 ;       % (Hz)
w = 2*pi.*f ;       % (rad/s)
C = 100*M ;
for i = 1:length(f)
    x = (-w(i)^2*M + 1i*w(i)*C + K)\F ;
    p(i) = (k/2)*abs(x(1)-x(2))/A ;
end
%% 6 Elements (closed open)
fprintf('6 Element Results (closed open):\n') ;
elements = 6 ;
k = rho*c^2*A/(Le/(2*elements)) ;   % (N/m)
M = m/elements*eye(elements) ;      % (kg)
K = [k/2 -k/2 0 0 0 0 ; ...
    -k/2 k -k/2 0 0 0 ; ...
    0 -k/2 k -k/2 0 0 ; ...
    0 0 -k/2 k -k/2 0 ; ...
    0 0 0 -k/2 k -k/2 ; ...
    0 0 0 0 -k/2 k/2+k] ;           % (N/m)
[~, D] = eig(M\K) ;
fn = 1/(2*pi)*sqrt(diag(D))         % (Hz)

F = zeros(elements,1) ;
F(1) = A ;
f = 0:1:500 ;       % (Hz)
w = 2*pi.*f ;       % (rad/s)
C = 100*M ;
for i = 1:length(f)
    x = (-w(i)^2*M + 1i*w(i)*C + K)\F ;
    p(i) = (k/2)*abs(x(1)-x(2))/A ;
end
%% 6 Elements
fprintf('6 Element Results:\n') ;
elements = 6 ;
k = rho*c^2*A/(Le/(2*elements)) ;   % (N/m)
M = m/elements*eye(elements) ;      % (kg)
K = [kv*k/(kv+k)+k/2 -k/2 0 0 0 0 ;...
    -k/2 k -k/2 0 0 0 ;...
    0 -k/2 k -k/2 0 0 ;...
    0 0 -k/2 k -k/2 0 ;...
    0 0 0 -k/2 k -k/2 ;...
    0 0 0 0 -k/2 k/2+k] ;           % (N/m)
[~, D] = eig(inv(M)*K) ;
fn = 1/(2*pi)*sqrt(diag(D))         % (Hz)

F = zeros(elements,1) ;
F(1) = A ;
f = 0:1:500 ;       % (Hz)
w = 2*pi.*f ;       % (rad/s)

figure
hold on
% Plotting pressure at 5-6 for range of C to M proportionalities
for a = 25:25:150
    C = a*M ;
    for i = 1:length(f)
        x = (-w(i)^2*M + 1i*w(i)*C + K)\F ; % Calculate motion given force input
        p12(i) = (k/2)*abs(x(1)-x(2))/A ;
        p23(i) = (k/2)*abs(x(2)-x(3))/A ;
        p34(i) = (k/2)*abs(x(3)-x(4))/A ;
        p45(i) = (k/2)*abs(x(4)-x(5))/A ;
        p56(i) = (k/2)*abs(x(5)-x(6))/A ;
    end
    plot(f,p56) ;
    
end
xlabel('Frequency [Hz]')
ylabel('Pressure Magnitude')
lgd1 = legend('25','50','75','100','125','150') ;
lgd1.Title.String = "\alpha" ;
lgd1.Title.FontSize = 12 ;

%% Plotting spectrums at positions
figure
max_f1 = max([max(p12(1:250)) max(p23(1:250)) max(p34(1:250)) max(p45(1:250)) max(p56(1:250))]) ;
max_f2 = max([max(p12(250:500)) max(p23(250:500)) max(p34(250:500)) max(p45(250:500)) max(p56(250:500))]) ;
hold on
plot(f,p12/max_f1) ;
plot(f,p23/max_f1) ;
plot(f,p34/max_f1) ;
plot(f,p45/max_f1) ;
plot(f,p56/max_f1) ;
xlabel('Frequency (Hz)') ;
ylabel('Amplitude') ;
legend('1-2','2-3','3-4','4-5','5-6') ;

%% Continious frequency and amplitudes
n = 1 ;    % First natural frequency
f_c = c/(4*Le)*(2*n-1) ;
ky = pi/2 * (2*n-1)/Le ;
x = [.0799 .1814 .2829 .3844 .4859]+.875*.0254 ;
amps = abs(sin(ky*x))/max(abs(sin(ky*x)))
figure
subplot(1,2,1) ;
x = linspace(0,Le,1000) ;
hold on
plot(x-0.875*0.0254,1/sin(ky*(.4859+.875*.0254))*abs(sin(ky*x))) ;
plot([.0799 .1814 .2829 .3844 .4859],[.2643 .5056 .7228 .8928 1], '.r')
xlabel('x [m]')
ylabel('Normalized Pressure [-]')
title('Pressure Shape, Mode 1')

n = 2 ;    % Second natural frequency
f_c = c/(4*Le)*(2*n-1) ;
ky = pi/2 * (2*n-1)/Le ;
x = [.0799 .1814 .2829 .3844 .4859]+.875*.0254 ;
amps = abs(sin(ky*x))/max(abs(sin(ky*x)))
subplot(1,2,2) ;
x = linspace(0,Le,1000) ;
hold on
plot(x-0.875*0.0254,1/sin(ky*(.1814+.875*.0254))*abs(sin(ky*x))) ;
plot([.0799 .1814 .2829 .3844 .4859],[.7074 1 .7121 .0385 .7053], '.r')
xlabel('x [m]')
ylabel('Normalized Pressure [-]')
title('Pressure Shape, Mode 2')
