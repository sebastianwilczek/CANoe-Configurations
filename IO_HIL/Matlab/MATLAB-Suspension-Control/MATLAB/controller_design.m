%% Create linear system from full car model at steady state suspension position
warning off;

clear sys_full
if( ~exist('sys_full','var') )
    % Create linear system around stationary point
    load_system('vehicle_dynamics_fullcar');
    cs = getActiveConfigSet('vehicle_dynamics_fullcar');
    mdl_cs = cs.copy;
    set_param(mdl_cs,'SolverType','Variable-step',...
         'Solver','ode23tb')
    simout = sim('vehicle_dynamics_fullcar',mdl_cs);
    opx_full = simout.find('xFinal').'; % Get states at stationary point
    u = zeros(12,1);                    % Inputs zero at linearization point
    iu = (1:12).';                      % Use all inputs = 0 
    ix = (8:14).';
    [opx_full,u,y,dx] = trim('vehicle_dynamics_fullcar',opx_full,u,[],ix,iu,[]);
    opy_full = y((length(opx_full)+1):end);
    opy_full(5:end)=0;
    sys_full=linmod('vehicle_dynamics_fullcar',opx_full,zeros(12,1));
    ssys_full = ss(sys_full.a,sys_full.b(:,1:4),sys_full.c((length(opx_full)+1):end,:),sys_full.d((length(opx_full)+1):end,1:4));
    close_system('vehicle_dynamics_fullcar');
end
warning on;


%% Controller & observer design

% Input/Output indices of simulink model
in_input = 1:4;
in_dist = 5:12;
    
out_measure = [1:24] + 14;
out_perform = [1:4, 5:8, 9:12, 13:16] + 14;
    
% Dynamics
A = sys_full.a;  % Dynamic matrix
B = sys_full.b(:,in_input); % Input matrix
G = sys_full.b(:,in_dist); % Disturbance matrix
    
% Measurements
C = sys_full.c(out_measure,:); % Measure matrix
D = sys_full.d(out_measure,in_input); % Direct feedthrough matrix
H = sys_full.d(out_measure,in_dist); % Direct measurement disturbance

% Performance outputs
C_perf = sys_full.c(out_perform,:); % Measure matrix
D_perf = sys_full.d(out_perform,in_input); % Direct feedthrough matrix
H_perf = sys_full.d(out_perform,in_dist); % Direct measurement disturbance
    
%% ========== Kalman observer design
sys_obsv_design = ss(A, [B G], C, [D H]); % Plant for observer design
Q = [ones(1,4),ones(1,4)]*1e-8; % Disturbance noise covariance matrix

% ---------- Measurement noise covariance matrix
% The first 8 measurements are suspension deflections and
% deflection velocities. These measurements are almost noise free due to
% the possibility of using digital sensors.
% The last 11 measurements are values from the accelerometers, thus
% noisy.
R_deflection    = ones(1,4)*1e-7;   % Suspension deflection noise strength
R_deflectionvel = ones(1,4)*1e-6;   % Suspension deflection velocity noise strength
R_wheelacc      = ones(1,4)*1e-2;
R_wheelvel      = ones(1,4)*1e-3;
R_bodyacc       = ones(1,4)*1e-2;
R_bodyvel       = ones(1,4)*1e-3;

R = [R_deflection, R_deflectionvel, R_bodyvel, R_bodyacc, R_wheelvel,R_wheelacc];

% Observer synthesis
[sys_estimator,L_estimator,P_ric] = kalman(sys_obsv_design,diag(Q),diag(R));

%% ========== LQR controller design
sys_ctrl_design = ss(A, B, C_perf, D_perf);
R = [1/900^2, 1/900^2, 1/900^2, 1/900^2]; % Input penalizing weights
Q_deflection = [0.1, 0.1, 0.1, 0.1]; % Penalties for suspension deflections
Q_deflectionvel = [1, 1, 1, 1]*0; % Penalties for suspension deflection velocities
Q_bodyvel = [1/0.008, 1/0.008, 1/0.008, 1/0.008]; % Penalties for vertical body corner velocities
Q_bodyacc = [1/1^2, 1/1^2, 1/1^2, 1/1^2]; % Penalties for vertical body corner accelerations
Q = [Q_deflection, Q_deflectionvel, Q_bodyvel, Q_bodyacc];

% Controller synthesis
K_lqr = lqry(sys_ctrl_design, diag(Q), diag(R));

% Closed loop system
sys_cl = ss( A-B*K_lqr, G, C-D*K_lqr, H );

% System poles
poles_lqr = eig(A-B*K_lqr);

%% ========== Observer design (pole placement)
% Desired poles
poles = -3*max(abs(real(poles_lqr)))*linspace(1,1.1,numel(poles_lqr));
%L_full = place(A.', C.',poles).';
L_full = place(A.', C.',3*real(poles_lqr)).';
sys_estimator = estim(ssys_full,L_full,1:size(ssys_full.C,1),1:size(ssys_full.B,2));

% Draw pole-zero map for observer and closed loop system
%pzmap(sys_cl, sys_estimator);
%legend show;